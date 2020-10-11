/*
	AST evaluation header DCC
    Copyright (C) 2020  Daan Oosterveld
    DCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    DCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "node.h"
#include "errorhandling.h"
#include "eval.h"
#include "symbols.h"
#include "identifier.h"
#include "global_eval.h"
#include "string_eval.h"
#include "struct.h"

int DEBUG_EVAL=0;

struct astlist *eval(struct astlist *ast);
struct astnode *eval_global(struct astnode *ast);
struct astlist *eval_arguments(struct astnode *ast);
struct astnode *eval_statement(struct astnode *ast);
struct astnode *eval_expression(struct astnode *ast);
struct astlist *eval_expression_list(struct astlist *list);
struct astnode *eval_lvalue(struct astnode *ast);
struct astnode *eval_jz(struct astnode *exp,int label);

int eval_type(struct typelist *type);

int global_label_count;
int max_stack;

struct typelist *function_type;

struct astlist *eval(struct astlist *ast)
{
	if(ast==0)
		return global_eval_gen();
	if(DEBUG_EVAL)
		fprintf(stderr,"evaluating top\n");
	struct astnode *node=eval_global(ast->child);
	return newastlist(GEN_LIST,ast->line,node,eval(ast->next));
}

struct astnode *eval_global(struct astnode *ast)
{
	if(ast->id==SYM_FUNCTION_DEFENITION)
	{
		if(DEBUG_EVAL)
			fprintf(stderr,"evaluating function definition %s\n",(char*)ast->child[0]);
		global_eval(ast->child[0],ast->type,1,0);
		function_type=ast->type->next;
		struct astnode *compound=ast->child[2];
		struct astnode *node=newnode(GEN_FUNCTION,ast->line,5);
		node->child[0]=ast->child[0];
		
		node->child[1]=eval_arguments(ast->type->val);
		enter_block();
		node->child[3]=eval_statement(compound->child[0]);
		node->child[2]=newconst(SYM_NUM,ast->line,leave_block()+max_stack);
		node->child[4]=local_strings;
		local_strings=NULL;
		str_count=0;
		remove_arguments();
		return node;
	}
	else if(ast->id==SYM_GLOBAL_DECLARATION)
	{
		if(ast->child[2]!=NULL)
			global_eval(ast->child[0],ast->type,1,((struct constnode *)ast->child[2])->val);
		else 
			global_eval(ast->child[0],ast->type,0,0);
	}
	return NULL;
}

struct astlist *eval_arguments(struct astnode *ast)
{
	if(ast==NULL)
		return NULL;
	struct astlist *last=NULL;
	struct astlist **start=&last;
	if(DEBUG_EVAL)
		fprintf(stderr,"evaluating function arguments\n");
	for(struct astnode *list=ast;list!=NULL;list=list->child[2])
	{
		if(DEBUG_EVAL)
			fprintf(stderr,"evaluating function argument\n");
			add_argument_id(list->child[0],list->type);
			
			struct astlist *new=newastlist(GEN_ARG+eval_type(ast->type),ast->line,list->child[0],NULL);
			if(last==NULL)
				last=new;
			else 
			{
				last->next=new;
				last=last->next;
			}
	}
	if(DEBUG_EVAL)
		fprintf(stderr,"evaluating function argument\n");
	return *start;
}

int continue_label;
int break_label;
int switch_label;

struct astnode *eval_statement(struct astnode *ast)
{
	if(DEBUG_EVAL)
		fprintf(stderr,"evaluating statement\n");
	if(ast==NULL)
		return NULL;
	switch(ast->id)
	{
		case SYM_COMPOUND_STATEMENT:
		{
			int local_max=max_stack;
			max_stack=0;
			enter_block();
			struct astnode *node=eval_statement(ast->child[0]);
			int stack=leave_block()+max_stack;
			if(stack>local_max)
				max_stack=stack;
			else 
				max_stack=local_max;
			return node;
		}
		
		case SYM_BLOCK_ITEM_LIST:
		{
			struct astlist *list=(void*)ast;
			struct astnode *child=eval_statement(list->child);
			if(child==NULL)
				return eval_statement(list->next);
			else if(child->id==GEN_LIST)
			{
				struct astlist *new;
				for(new=(void*)child;new->next!=NULL;new=new->next);
				new->next=eval_statement(list->next);
				return (void*)child;
			}
			else
				return (void*)newastlist(GEN_LIST,list->line,child,eval_statement(list->next));
				
		}
		case SYM_RETURN:
		{
			struct astnode *node=newnode(GEN_RETURN+eval_type(function_type),ast->line,1);
			node->child[0]=eval_expression(ast->child[0]);
			return node;
		}
		case SYM_IF:
		{
			int label_count=global_label_count;
			global_label_count=global_label_count+1;
			
			/*struct astnode *node=newnode(GEN_JZ+gen_size->int_size,ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=newconst(GEN_LABEL,ast->line,label_count);*/
			struct astnode *node=eval_jz(ast->child[0],label_count);
			
			struct astnode *child=eval_statement(ast->child[1]);
			
			struct constnode *label=newconst(GEN_LABEL,ast->line,label_count);
			
			struct astlist *list=newastlist(GEN_LIST,ast->line,node,
									newastlist(GEN_LIST,ast->line,child,
									newastlist(GEN_LIST,ast->line,label,NULL)));
			return (void*)list;
		}
		case SYM_IF_ELSE:
		{
			int label_count=global_label_count;
			global_label_count=global_label_count+2;
			
			/*struct astnode *node=newnode(GEN_JZ+gen_size->int_size,ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=newconst(GEN_LABEL,ast->line,label_count);*/
			struct astnode *node=eval_jz(ast->child[0],label_count);
			
			struct astnode *child1=eval_statement(ast->child[1]);
			
			struct astnode *jump=newnode(GEN_JUMP,ast->line,1);
			jump->child[0]=newconst(GEN_LABEL,ast->line,label_count+1);
			
			struct constnode *label1=newconst(GEN_LABEL,ast->line,label_count);
			
			struct astnode *child2=eval_statement(ast->child[2]);
			
			struct constnode *label2=newconst(GEN_LABEL,ast->line,label_count+1);
			
			struct astlist *list=newastlist(GEN_LIST,ast->line,node,
									newastlist(GEN_LIST,ast->line,child1,
									newastlist(GEN_LIST,ast->line,jump,
									newastlist(GEN_LIST,ast->line,label1,
									newastlist(GEN_LIST,ast->line,child2,
									newastlist(GEN_LIST,ast->line,label2,NULL))))));
			return (void*)list;
		}
		case SYM_WHILE:
		{
			int label_count=global_label_count;
			int c_label=continue_label;
			int b_label=break_label;
			global_label_count=global_label_count+2;
			continue_label=label_count;
			break_label=label_count+1;
			
			struct constnode *label1=newconst(GEN_LABEL,ast->line,label_count);
			
			/*struct astnode *node=newnode(GEN_JZ+gen_size->int_size,ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=newconst(GEN_LABEL,ast->line,label_count+1);*/
			struct astnode *node=eval_jz(ast->child[0],label_count+1);
			
			struct astnode *child=eval_statement(ast->child[1]);
			
			struct astnode *jump=newnode(GEN_JUMP,ast->line,1);
			jump->child[0]=newconst(GEN_LABEL,ast->line,label_count);
			
			struct constnode *label2=newconst(GEN_LABEL,ast->line,label_count+1);
			
			
			struct astlist *list=newastlist(GEN_LIST,ast->line,label1,
									newastlist(GEN_LIST,ast->line,node,
									newastlist(GEN_LIST,ast->line,child,
									newastlist(GEN_LIST,ast->line,jump,
									newastlist(GEN_LIST,ast->line,label2,NULL)))));
			continue_label=c_label;
			break_label=b_label;
			return (void*)list;
		}
		case SYM_DO_WHILE:
		{
			int label_count=global_label_count;
			int c_label=continue_label;
			int b_label=break_label;
			global_label_count=global_label_count+2;
			continue_label=label_count;
			break_label=label_count+1;
			
			struct constnode *label1=newconst(GEN_LABEL,ast->line,label_count);
			
			struct astnode *child=eval_statement(ast->child[1]);
			
			struct astnode *node=newnode(GEN_JNZ+gen_size->int_size,ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=newconst(GEN_LABEL,ast->line,label_count);
			
			struct constnode *label2=newconst(GEN_LABEL,ast->line,label_count+1);
			
			
			struct astlist *list=newastlist(GEN_LIST,ast->line,label1,
									newastlist(GEN_LIST,ast->line,child,
									newastlist(GEN_LIST,ast->line,node,
									newastlist(GEN_LIST,ast->line,label2,NULL))));
			
			continue_label=c_label;
			break_label=b_label;
			return (void*)list;
		}
		case SYM_FOR:
		{
			int local_max=max_stack;
			max_stack=0;
			enter_block();
			
			
			int label_count=global_label_count;
			int c_label=continue_label;
			int b_label=break_label;
			global_label_count=global_label_count+3;
			continue_label=label_count+1;
			break_label=label_count+2;
			
			
			struct astnode *start=eval_statement(ast->child[0]);
			
			struct constnode *label1=newconst(GEN_LABEL,ast->line,label_count);
			
			/*struct astnode *condition=newnode(GEN_JZ+gen_size->int_size,ast->line,2);
			condition->child[0]=eval_expression(ast->child[1]);
			condition->child[1]=newconst(GEN_LABEL,ast->line,label_count+2);*/
			struct astnode *condition=eval_jz(ast->child[1],label_count+2);
			
			struct astnode *statement=eval_statement(ast->child[3]);
			
			struct constnode *label2=newconst(GEN_LABEL,ast->line,label_count+1);
			
			struct astnode *update=eval_expression(ast->child[2]);
			
			struct astnode *jump=newnode(GEN_JUMP,ast->line,1);
			jump->child[0]=newconst(GEN_LABEL,ast->line,label_count);
			
			struct constnode *label3=newconst(GEN_LABEL,ast->line,label_count+2);
			
			struct astlist *list=newastlist(GEN_LIST,ast->line,start,
									newastlist(GEN_LIST,ast->line,label1,
									newastlist(GEN_LIST,ast->line,condition,
									newastlist(GEN_LIST,ast->line,statement,
									newastlist(GEN_LIST,ast->line,label2,
									newastlist(GEN_LIST,ast->line,update,
									newastlist(GEN_LIST,ast->line,jump,
									newastlist(GEN_LIST,ast->line,label3,NULL))))))));
									
			continue_label=c_label;
			break_label=b_label;
			
			
			int stack=leave_block()+max_stack;
			if(stack>local_max)
				max_stack=stack;
			else 
				max_stack=local_max;
			
			return (void *)list;
		}
		case SYM_BREAK:
		{
			struct astnode *jump=newnode(GEN_JUMP,ast->line,1);
			jump->child[0]=newconst(GEN_LABEL,ast->line,break_label);
			return jump;
		}
		case SYM_CONTINUE:
		{
			struct astnode *jump=newnode(GEN_JUMP,ast->line,1);
			jump->child[0]=newconst(GEN_LABEL,ast->line,continue_label);
			return jump;
		}
		case SYM_SWITCH:
		{
			int label_count=global_label_count;
			int s_label=switch_label;
			global_label_count=global_label_count+1;
			switch_label=label_count;
			
			struct constnode *label=newconst(GEN_LABEL,ast->line,label_count);
			
			struct astnode *node=newnode(GEN_CASE,ast->line,3);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=label;
			node->child[2]=ast->child[2];
			
			struct astnode *child=eval_statement(ast->child[1]);
			
			struct astlist *list=newastlist(GEN_LIST,ast->line,node,
									newastlist(GEN_LIST,ast->line,child,
									newastlist(GEN_LIST,ast->line,label,NULL)));
			
			switch_label=s_label;
			return (void*)list;
		}
		case SYM_CASE:
		case SYM_DEFAULT:
		{
			struct constnode *label=newconst(GEN_LABEL,ast->line,((struct constnode *)ast->child[2])->val);
			struct astnode *statement=eval_statement(ast->child[1]);
			struct astlist *list=newastlist(GEN_LIST,ast->line,label,
									newastlist(GEN_LIST,ast->line,statement,NULL));
			return (void *)list;
		}
		case SYM_SWITCH_BREAK:
		{
			struct astnode *jump=newnode(GEN_JUMP,ast->line,1);
			jump->child[0]=newconst(GEN_LABEL,ast->line,switch_label);
			return jump;
		}
		case SYM_LOCAL_DECLARATION:
		{
			if(DEBUG_EVAL)
			{
				fprintf(stderr,"eval local declaration\n");
				print_type(ast->type);
			}
			add_local_id(ast->child[0],ast->type);
			if(ast->child[2]!=NULL)
			{
				if(DEBUG_EVAL)
					fprintf(stderr,"initialize local declaration\n");
				struct astnode *node=newnode(GEN_ASSIGN+eval_type(ast->type),ast->line,2);
				void *ptr;
				node->child[0]=newconst(GEN_ADDRL+P+gen_size->int_size,ast->line,find_local_id(ast->child[0],(void *)&ptr));
				node->child[1]=eval_expression(ast->child[2]);
				return node;
			}
			return NULL;
		}
		default:
		{
			return eval_expression(ast);
		}
	}
}

int local_array_size(struct typelist *type,int first);
struct astnode *eval_expression(struct astnode *ast)
{
	if(ast==NULL)
		return NULL;
	if(DEBUG_EVAL)
		fprintf(stderr,"evaluating expression\n");
	switch(ast->id)
	{
		case SYM_NUM:
		{
			struct constnode *node=newconst(GEN_NUM+eval_type(ast->type),ast->line,((struct constnode *)ast)->val);
			return (void*)node;
		}
		case SYM_ID:
		case SYM_IDL:
		{
			void *ptr;
			struct constnode *node;
			if(ast->type->type!=TYPE_ARRAY)
				node=newconst(GEN_LOADL+eval_type(ast->type),ast->line,find_local_id(ast->child[0],(void*)&ptr));
			else 
				node=newconst(GEN_ADDRL+eval_type(ast->type),ast->line,find_local_id(ast->child[0],(void*)&ptr));
			return (void*)node;
		}
		case SYM_IDF:
		{
			void *ptr;
			struct constnode *node=newconst(GEN_LOADF+eval_type(ast->type),ast->line,find_argument_id(ast->child[0],(void*)&ptr));
			return (void*)node;
		}
		case SYM_IDG:
		{
			struct astnode *node;
			if(ast->type->type!=TYPE_ARRAY)
				node=newnode(GEN_LOADG+eval_type(ast->type),ast->line,1);
			else 
				node=newnode(GEN_ADDRG+eval_type(ast->type),ast->line,1);
			node->child[0]=ast->child[0];
			return (void*)node;
		}
		case SYM_STRING:
		{
			return (void*)newconst(GEN_STRING+P+gen_size->pointer_size,ast->line,add_string(ast->child[0]));
		}
		case SYM_SIZEOF:
		{
			return (void*)newconst(GEN_NUM+gen_size->int_size,ast->line,local_array_size(ast->child[1],0));
		}
		case SYM_FUNCTION_CALL:
		{
			struct astnode *node=newnode(GEN_CALL+eval_type(ast->type),ast->line,2);
			node->child[0]=((struct astnode *)ast->child[0])->child[0];
			node->child[1]=eval_expression_list(ast->child[1]);
			return node;
		}
		case SYM_VFUNCTION_CALL:
		{
			struct astnode *node=newnode(GEN_VCALL+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression_list(ast->child[1]);
			return node;
		}
		case SYM_ARRAY:
		{
			//struct typelist *lefttype=((struct astnode *)ast->child[0])->type;
			struct typelist *righttype=((struct astnode *)ast->child[1])->type;
			
			
			struct astnode *node;
			struct astnode *mul_right=eval_expression(ast->child[1]);
			struct constnode *c=newconst(GEN_NUM+eval_type(righttype),ast->line,local_array_size(ast->type,0));
			struct astnode *mul=newnode(GEN_MUL+eval_type(righttype),ast->line,2);
			struct astnode *add_left=eval_expression(ast->child[0]);
			struct astnode *add=newnode(GEN_ADD+P+gen_size->pointer_size,ast->line,2);
			if(ast->type->type!=TYPE_ARRAY)
			{
				node=newnode(GEN_INDIR+eval_type(ast->type),ast->line,1);
				node->child[0]=add;
			}
			else node=add;
			add->child[0]=add_left;
			add->child[1]=mul;
			mul->child[0]=c;
			mul->child[1]=mul_right;
			
			return node;
		}
		case SYM_INDIRECT:
		{
			struct astnode *node=newnode(GEN_INDIR+eval_type(ast->type),ast->line,1);
			node->child[0]=eval_expression(ast->child[0]);
			return node;
		}
		case SYM_DOT:
		{
			int offset=0;
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type;
			find_struct_member(struct_type->structname,ast->child[1],&offset);
			
			struct astnode *node;
		
			struct astnode *add=newnode(GEN_ADD+P+gen_size->pointer_size,ast->line,2);
			struct astnode *mem=eval_lvalue(ast->child[0]);
			struct constnode *num=newconst(GEN_NUM+I+gen_size->int_size,ast->line,offset);
			if((ast->type->type==TYPE_STRUCT)||(ast->type->type==TYPE_ARRAY))
				node=add;
			else 
			{
				node=newnode(GEN_INDIR+eval_type(ast->type),ast->line,1);
				node->child[0]=add;
			}
			add->child[0]=mem;
			add->child[1]=num;
			return node;
		}
		case SYM_ARROW:
		{
			int offset=0;
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type->next;
			find_struct_member(struct_type->structname,ast->child[1],&offset);
			struct astnode *node;
			struct astnode *add=newnode(GEN_ADD+P+gen_size->pointer_size,ast->line,2);
			struct astnode *mem=eval_expression(ast->child[0]);
			struct constnode *num=newconst(GEN_NUM+I+gen_size->int_size,ast->line,offset);
			if((ast->type->type==TYPE_STRUCT)||(ast->type->type==TYPE_ARRAY))
				node=add;
			else 
			{
				node=newnode(GEN_INDIR+eval_type(ast->type),ast->line,1);
				node->child[0]=add;
			}
			add->child[0]=mem;
			add->child[1]=num;
			return node;
		}
		case SYM_ADDRESS:
		{
			return eval_lvalue(ast->child[0]);
		}
		case SYM_NEGATE:
		{
			struct astnode *node=newnode(GEN_NEG+eval_type(ast->type),ast->line,1);
			node->child[0]=eval_expression(ast->child[0]);
			return node;
		}
		case '~' :
		{
			struct astnode *node=newnode(GEN_BNOT+eval_type(ast->type),ast->line,1);
			node->child[0]=eval_expression(ast->child[0]);
			return node;
		}
		case '!' :
		{
			struct astnode *node=newnode(GEN_LNOT+eval_type(ast->type),ast->line,1);
			node->child[0]=eval_expression(ast->child[0]);
			return node;
		}
		case SYM_CAST:
		{
			struct astnode *node=newnode(GEN_CAST+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=newconst(GEN_NUM,ast->line,eval_type(((struct constnode *)ast->child[0])->type));
			return node;
		}
		case '*':
		{
			struct astnode *node=newnode(GEN_MUL+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case '/':
		{
			struct astnode *node=newnode(GEN_DIV+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case '+':
		case '-':
		{
			struct typelist *lefttype=((struct astnode *)ast->child[0])->type;
			struct typelist *righttype=((struct astnode *)ast->child[1])->type;
			
			int id=ast->id=='+'?GEN_ADD:GEN_SUB;
			struct astnode *node=newnode(id+eval_type(ast->type),ast->line,2);
			
			if(lefttype->type==TYPE_POINTER)
			{
				if(DEBUG_EVAL)
					fprintf(stderr,"type pointer in add/sub\n");
				int size=eval_type(lefttype->next)&0x1f;
				if(size==1)
				{
					node->child[1]=eval_expression(ast->child[1]);
				}
				else if(((struct astnode *)ast->child[1])->id==SYM_NUM)
				{
					node->child[1]=newconst(GEN_NUM+eval_type(righttype),ast->line,((struct constnode *)ast->child[1])->val*size);
				}
				else
				{
					struct astnode *mul=newnode(GEN_MUL+eval_type(righttype),ast->line,2);
					mul->child[1]=newconst(GEN_NUM+eval_type(righttype),ast->line,size);
					mul->child[0]=eval_expression(ast->child[1]);
					node->child[1]=mul;
				}
				node->child[0]=eval_expression(ast->child[0]);
			}
			else if(righttype->type==TYPE_POINTER)
			{
				if(DEBUG_EVAL)
					fprintf(stderr,"type pointer in add/sub\n");
				int size=eval_type(lefttype->next)&0x1f;
				if(size==1)
				{
					node->child[0]=eval_expression(ast->child[0]);
				}
				else if(((struct astnode *)ast->child[0])->id==SYM_NUM)
				{
					node->child[0]=newconst(GEN_NUM+eval_type(lefttype),ast->line,((struct constnode *)ast->child[0])->val*size);
				}
				else
				{
					struct astnode *mul=newnode(GEN_MUL+eval_type(lefttype),ast->line,2);
					mul->child[1]=newconst(GEN_NUM+eval_type(lefttype),ast->line,size);
					mul->child[0]=eval_expression(ast->child[0]);
					node->child[0]=mul;
				}
				node->child[1]=eval_expression(ast->child[1]);
			}
			else 
			{
				node->child[0]=eval_expression(ast->child[0]);
				node->child[1]=eval_expression(ast->child[1]);
			}
			return node;
		}
		case '>':
		{
			struct astnode *node=newnode(GEN_GT+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case '<':
		{
			struct astnode *node=newnode(GEN_LT+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case SYM_LE:
		{
			struct astnode *node=newnode(GEN_LE+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case SYM_GE:
		{
			struct astnode *node=newnode(GEN_GE+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case SYM_NE:
		{
			struct astnode *node=newnode(GEN_NE+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case SYM_EQ:
		{
			struct astnode *node=newnode(GEN_EQ+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case '&':
		{
			struct astnode *node=newnode(GEN_BAND+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case SYM_LOR:
		{
			struct astnode *node=newnode(GEN_LOR+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case SYM_LAND:
		{
			struct astnode *node=newnode(GEN_LAND+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		case SYM_SEL:
		{
			struct astnode *node=newnode(GEN_SEL+eval_type(ast->type),ast->line,3);
			node->child[0]=eval_expression(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			node->child[2]=eval_expression(ast->child[2]);
			return node;
		}
		case '=':
		{
			if(DEBUG_EVAL)
			{
				fprintf(stderr,"evaluating =\n");
				print_type(ast->type);
			}
			struct astnode *node=newnode(GEN_ASSIGN+eval_type(ast->type),ast->line,2);
			node->child[0]=eval_lvalue(ast->child[0]);
			node->child[1]=eval_expression(ast->child[1]);
			return node;
		}
		default:
			fprintf(stderr,"%d\n",ast->id);
			error("expected valid expression",ast->line);
			exit(-1);
	}
}

struct astlist *eval_expression_list(struct astlist *list)
{
	if(DEBUG_EVAL)
		fprintf(stderr,"evaluating expression list\n");
	if(list==NULL)
		return NULL;
	struct typelist *type=((struct astnode *)list->child)->type;
	return newastlist(GEN_ARG+eval_type(type),list->line,
						eval_expression(list->child),
						eval_expression_list(list->next));
}

struct astnode *eval_lvalue(struct astnode *ast)
{
	if(DEBUG_EVAL)
		fprintf(stderr,"evaluating lvalue\n");
	switch(ast->id)
	{
		case SYM_ID:
		case SYM_IDL:
		{
			void *ptr;
			struct constnode *node=newconst(GEN_ADDRL+P+gen_size->pointer_size,ast->line,find_local_id(ast->child[0],(void*)&ptr));
			return (void*)node;
		}
		case SYM_IDF:
		{
			void *ptr;
			struct constnode *node=newconst(GEN_ADDRF+P+gen_size->pointer_size,ast->line,find_argument_id(ast->child[0],(void*)&ptr));
			return (void*)node;
		}
		case SYM_IDG:
		{
			struct astnode *node=newnode(GEN_ADDRG+P+gen_size->pointer_size,ast->line,1);
			node->child[0]=ast->child[0];
			return (void*)node;
		}
		case SYM_INDIRECT:
		{
			return eval_expression(ast->child[0]);
		}
		case SYM_ARRAY:
		{
			struct typelist *righttype=((struct astnode *)ast->child[1])->type;
			if(DEBUG_EVAL)
				print_type(ast->type);
			
			struct astnode *add=newnode(GEN_ADD+P+gen_size->pointer_size,ast->line,2);
			struct astnode *add_left=eval_expression(ast->child[0]);
			struct astnode *mul=newnode(GEN_MUL+eval_type(righttype),ast->line,2);
			struct constnode *c=newconst(GEN_NUM+eval_type(righttype),ast->line,local_array_size(ast->type,0));
			struct astnode *mul_right=eval_expression(ast->child[1]);

			add->child[0]=add_left;
			add->child[1]=mul;
			mul->child[0]=c;
			mul->child[1]=mul_right;
			
			return add;
		}
		case SYM_DOT:
		{
			if(DEBUG_EVAL)
			{
				fprintf(stderr,"evaluating .\n");
				print_type(ast->type);
			}
			int offset=0;
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type;
			find_struct_member(struct_type->structname,ast->child[1],&offset);
			struct astnode *add=newnode(GEN_ADD+P+gen_size->pointer_size,ast->line,2);
			struct astnode *mem=eval_lvalue(ast->child[0]);
			struct constnode *num=newconst(GEN_NUM+I+gen_size->int_size,ast->line,offset);
			add->child[0]=mem;
			add->child[1]=num;
			return add;
		}
		case SYM_ARROW:
		{
			if(DEBUG_EVAL)
			{
				fprintf(stderr,"evaluating ->\n");
				print_type(ast->type);
			}
			int offset=0;
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type->next;
			find_struct_member(struct_type->structname,ast->child[1],&offset);
			struct astnode *add=newnode(GEN_ADD+P+gen_size->pointer_size,ast->line,2);
			struct astnode *mem=eval_expression(ast->child[0]);
			struct constnode *num=newconst(GEN_NUM+I+gen_size->int_size,ast->line,offset);
			add->child[0]=mem;
			add->child[1]=num;
			return add;
		}
		default:
			error("expected valid lvalue",ast->line);
			exit(-1);
	}
}

int eval_type(struct typelist *type)
{
	if(type==NULL)
	{
		if(DEBUG_EVAL)
			fprintf(stderr,"typelist in eval_type is NULL\n");
		return I+gen_size->int_size;
	}
	switch(type->type)
	{
		case TYPE_INT:
			return I+gen_size->int_size;
		case TYPE_POINTER:
		case TYPE_ARRAY:
			return P+gen_size->pointer_size;
		case TYPE_CHAR:
			return I+1;//By definition
		case TYPE_SHORT:
			return I+gen_size->short_size;
		case TYPE_LONG:
			return I+gen_size->long_size;
		case TYPE_VOID:
			return 0;
		default: 
			return I+gen_size->int_size;
	}
}

int add_string(char *str)
{
	struct string_list *ptr=calloc(2,sizeof(void *));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal Error: Out of memory");
		exit(-1);
	}
	ptr->next=local_strings;
	ptr->str=str;
	ptr->n=str_count;
	str_count=str_count+1;
	local_strings=ptr;
	return ptr->n;
}

struct astnode *eval_jz(struct astnode *exp,int label)
{
	switch(exp->id)
	{
		case SYM_EQ:	
		case SYM_NE:	
		case '<':	
		case SYM_LE:	
		case '>':	
		case SYM_GE:	
		{
			int id=GEN_JNE;
			switch(exp->id)
			{
				case SYM_EQ: 	id=GEN_JNE;	break;
				case SYM_NE:	id=GEN_JEQ;	break;	
				case '<':		id=GEN_JGE;	break;
				case SYM_LE:	id=GEN_JGT;	break;
				case '>':		id=GEN_JLE;	break;
				case SYM_GE:	id=GEN_JLT;	break;
				default:		fprintf(stderr,"Error eval jz\n");
			}
			struct astnode *jump=newnode(id+gen_size->int_size,exp->line,3);
			jump->child[0]=eval_expression(exp->child[0]);
			jump->child[1]=eval_expression(exp->child[1]);
			jump->child[2]=newconst(GEN_LABEL,exp->line,label);
			return jump;
		}
		default:
		{
			struct astnode *jump=newnode(GEN_JZ+gen_size->int_size,exp->line,2);
			jump->child[0]=eval_expression(exp);
			jump->child[1]=newconst(GEN_LABEL,exp->line,label);
			return jump;
		}
	}
}