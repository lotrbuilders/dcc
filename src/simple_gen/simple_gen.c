/*
	Generator DCC
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
#include "type.h"
#include "node.h"
#include "eval.h"
#include "simple_gen.h"
#include "string_eval.h"
#include "switch.h"

int gen_statement(struct astnode *ast);
int gen_expression(struct astnode *ast);
int gen_strings(struct string_list *list);

int is_terminal(struct astnode *ast);
struct terminal *next_terminal;
struct terminal term;

int GEN_DEBUG=0;
int global_label_count;
int frame_size;

int gen(struct astnode *ast)
{
	if(GEN_DEBUG)
		fprintf(stderr,"generating\n");
	if(ast==NULL)
		return 0;
	switch(ast->id)
	{
		case GEN_LIST:
		for(struct astlist *list=(void*)ast;list!=0;list=list->next)
		{
			gen(list->child);
		}
		break;
			
		case GEN_FUNCTION:
		{
			interface->function_prolog(ast->child[0],ast->child[1]);
			struct constnode *frame=ast->child[2];
			frame_size=frame->val;
			interface->enter_frame(frame_size);
			gen_statement(ast->child[3]);
			interface->function_epilog(ast->child[0]);
			gen_strings(ast->child[4]);
		}
		break;
		case GEN_EXTERN:
			interface->extern_global(ast->child[0]);
		break;
		
		case GEN_GLOBAL+I+1:
		case GEN_GLOBAL+I+2:
		case GEN_GLOBAL+I+4:
		case GEN_GLOBAL+I+8:
		case GEN_GLOBAL+P+2:
		case GEN_GLOBAL+P+4:
		case GEN_GLOBAL+P+8:
		{
			interface->define_global(ast->child[0],ast->id&0x1f,((struct constnode*)ast->child[1])->val);
		}
		break;
		
		case GEN_COMMON:
		{
			interface->common_global(ast->child[0],((struct constnode*)ast->child[1])->val);
		}
		break;
	}
	return 0;
}

int resolve_terminal();
int gen_statement(struct astnode *ast)
{
	if(ast==NULL)
		return 0;
	switch(ast->id)
	{
		case GEN_LIST:
		{
			for(struct astlist *list=(void*)ast;list!=NULL;list=list->next)
			{
				puts("");
				gen_statement(list->child);
			}
		}
		break;
		
		case GEN_JUMP:
		{
			int label=((struct constnode *)ast->child[0])->val;
			if(GEN_DEBUG)
				fprintf(stderr,"gen jump\n");
			interface->jump(label);
		}
		break;
		
		case GEN_LABEL:
		{
			int label=((struct constnode *)ast)->val;
			if(GEN_DEBUG)
				fprintf(stderr,"gen label\n");
			interface->label(label);
		}
		break;
		
		case GEN_RETURN:
		{
			interface->leave_frame(frame_size);
			interface->function_return(0);
		}
		break;
		
		case GEN_RETURN+I+1:
		case GEN_RETURN+I+2:
		case GEN_RETURN+I+4:
		case GEN_RETURN+I+8:
		case GEN_RETURN+P+2:
		case GEN_RETURN+P+4:
		case GEN_RETURN+P+8:
		{
			if(GEN_DEBUG)
				fprintf(stderr,"gen return\n");
			gen_expression(ast->child[0]);
			resolve_terminal();
			interface->leave_frame(frame_size);
			interface->function_return(ast->id&0x1f);
		}
		break;
		
		case GEN_JZ+I+2:
		case GEN_JZ+I+4:
		case GEN_JZ+I+8:
		{
			int label=((struct constnode *)ast->child[1])->val;
			if(GEN_DEBUG)
				fprintf(stderr,"gen jz\n");
			gen_expression(ast->child[0]);
			resolve_terminal();
			interface->jump_z(label);
		}
		break;
		
		case GEN_JNZ+I+2:
		case GEN_JNZ+I+4:
		case GEN_JNZ+I+8:
		{
			int label=((struct constnode *)ast->child[1])->val;
			if(GEN_DEBUG)
				fprintf(stderr,"gen jnz\n");
			gen_expression(ast->child[0]);
			resolve_terminal();
			interface->jump_nz(label);
		}
		break;
		
		case GEN_JEQ+I+2:
		case GEN_JEQ+I+4:
		case GEN_JEQ+I+8:
		case GEN_JNE+I+2:
		case GEN_JNE+I+4:
		case GEN_JNE+I+8:
		case GEN_JLE+I+2:
		case GEN_JLE+I+4:
		case GEN_JLE+I+8:
		case GEN_JLT+I+2:
		case GEN_JLT+I+4:
		case GEN_JLT+I+8:
		case GEN_JGE+I+2:
		case GEN_JGE+I+4:
		case GEN_JGE+I+8:
		case GEN_JGT+I+2:
		case GEN_JGT+I+4:
		case GEN_JGT+I+8:
		{
			if(GEN_DEBUG)
				fprintf(stderr,"gen jump if ==/!=/>/</>=/<= \n");
			int label=((struct constnode *)ast->child[2])->val;
			if(is_terminal(ast->child[1]))
			{
				gen_expression(ast->child[0]);
				resolve_terminal();
				gen_expression(ast->child[1]);
				interface->branch(ast->id,next_terminal,label);
				next_terminal=0;
			}
			else 
			{
				gen_expression(ast->child[1]);
				resolve_terminal();
				interface->save_top();
				gen_expression(ast->child[0]);
				resolve_terminal();
				interface->branch(ast->id,next_terminal,label);
				next_terminal=0;
			}
			break;
		}
		case GEN_CASE:
		{
			gen_expression(ast->child[0]);
			resolve_terminal();
			for(struct switchlist *list=ast->child[2];list!=NULL;list=list->next)
			{
				if(GEN_DEBUG)
					fprintf(stderr,"case:\n\tdefault: %d\n\t%p:%p\n",list->def,list,list->next);
				if(!list->def)
					interface->jump_case(list->val,list->label);
				if(list->def)
					interface->jump(list->label);
				else if(list->next==NULL)
				{
					int label=((struct constnode *)ast->child[1])->val;
					interface->jump(label);
				}
			}
		}
		break;
		
		default:
		{
			gen_expression(ast);
			resolve_terminal();
		}
	}
	return 0;
}



int is_terminal(struct astnode *ast)
{
	if(ast->id==GEN_NUM+I+1)
		ast->id=GEN_NUM+I+gen_size->int_size;
	switch(ast->id)
	{
		case GEN_LOADL+I+2:
		case GEN_LOADG+I+2:
		case GEN_LOADF+I+2:
			if(gen_size->int_size!=2)
				return 0;
			else 
				return interface->is_terminal(ast);
			
		case GEN_LOADL+I+4:
		case GEN_LOADG+I+4:
		case GEN_LOADF+I+4:
			if(gen_size->int_size!=4)
				return 0;
			else 
				return interface->is_terminal(ast);
			
		case GEN_LOADL+I+8:
		case GEN_LOADG+I+8:
		case GEN_LOADF+I+8:
			if(gen_size->int_size!=8)
				return 0;
			else 
				return interface->is_terminal(ast);
		
		case GEN_NUM+I+2:
		case GEN_NUM+I+4:
		case GEN_NUM+I+8:
		case GEN_NUM+P+2:
		case GEN_NUM+P+4:
		case GEN_NUM+P+8:
		
		case GEN_LOADL+P+2:
		case GEN_LOADL+P+4:
		case GEN_LOADL+P+8:
		case GEN_LOADG+P+2:
		case GEN_LOADG+P+4:
		case GEN_LOADG+P+8:
		case GEN_LOADF+P+2:
		case GEN_LOADF+P+4:
		case GEN_LOADF+P+8:
		
		case GEN_ADDRL+P+2:
		case GEN_ADDRL+P+4:
		case GEN_ADDRL+P+8:
		case GEN_ADDRF+P+2:
		case GEN_ADDRF+P+4:
		case GEN_ADDRF+P+8:
		case GEN_ADDRG+P+2:
		case GEN_ADDRG+P+4:
		case GEN_ADDRG+P+8:
		
		case GEN_STRING+P+2:
		case GEN_STRING+P+4:
		case GEN_STRING+P+8:
			return interface->is_terminal(ast);
	}
	return 0;
}

int is_address(struct astnode *ast)
{
	switch(ast->id)
	{
		case GEN_ADDRL+P+2:
		case GEN_ADDRL+P+4:
		case GEN_ADDRL+P+8:
		case GEN_ADDRF+P+2:
		case GEN_ADDRF+P+4:
		case GEN_ADDRF+P+8:
		case GEN_ADDRG+P+2:
		case GEN_ADDRG+P+4:
		case GEN_ADDRG+P+8:
			return 1;
		default:
			return 0;
	}
}

int gen_expression(struct astnode *ast)
{
	if(ast==NULL)
		return 0;
	if(GEN_DEBUG)
		fprintf(stderr,"gen expression\n");
	if(ast->id==GEN_NUM+I+1)
		ast->id=GEN_NUM+I+gen_size->int_size;
	switch(ast->id)
	{
		case GEN_NUM+P+2:
		case GEN_NUM+P+4:
		case GEN_NUM+P+8:
		case GEN_NUM+I+2:
		case GEN_NUM+I+4:
		case GEN_NUM+I+8:
		case GEN_LOADL+I+1:
		case GEN_LOADL+I+2:
		case GEN_LOADL+I+4:
		case GEN_LOADL+P+2:
		case GEN_LOADL+P+4:
		case GEN_LOADL+P+8:
		case GEN_ADDRL+P+2:
		case GEN_ADDRL+P+4:
		case GEN_ADDRL+P+8:
		case GEN_LOADF+I+1:
		case GEN_LOADF+I+2:
		case GEN_LOADF+I+4:
		case GEN_LOADF+P+2:
		case GEN_LOADF+P+4:
		case GEN_LOADF+P+8:
		case GEN_ADDRF+P+2:
		case GEN_ADDRF+P+4:
		case GEN_ADDRF+P+8:
		case GEN_STRING+P+2:
		case GEN_STRING+P+4:
		case GEN_STRING+P+8:
			if(GEN_DEBUG)
				fprintf(stderr,"gen num\n");
			term.type=ast->id;
			term.val=((struct constnode *)ast)->val;
			next_terminal=&term;
			//if(!is_terminal(ast))
			//	resolve_terminal();
			break;	
			
		case GEN_LOADG+I+1:
		case GEN_LOADG+I+2:
		case GEN_LOADG+I+4:
		case GEN_LOADG+I+8:
		case GEN_LOADG+P+2:
		case GEN_LOADG+P+4:
		case GEN_LOADG+P+8:
		case GEN_ADDRG+P+2:
		case GEN_ADDRG+P+4:
		case GEN_ADDRG+P+8:
			if(GEN_DEBUG)
				fprintf(stderr,"gen global\n");
			term.type=ast->id;
			term.ptr=ast->child[0];
			next_terminal=&term;
			//if(!is_terminal(ast))
			//	resolve_terminal();
			break;
			
	
		case GEN_NEG+I+2:
		case GEN_NEG+I+4:
		case GEN_NEG+I+8:
		case GEN_BNOT+I+2:
		case GEN_BNOT+I+4:
		case GEN_BNOT+I+8:
		case GEN_LNOT+I+2:
		case GEN_LNOT+I+4:
		case GEN_LNOT+I+8:
		case GEN_INDIR+I+1:
		case GEN_INDIR+I+2:
		case GEN_INDIR+I+4:
		case GEN_INDIR+I+8:
		case GEN_INDIR+P+2:
		case GEN_INDIR+P+4:
		case GEN_INDIR+P+8:
			if(GEN_DEBUG)
				fprintf(stderr,"gen -/~/! \n");
			gen_expression(ast->child[0]);
			interface->unary_expression(ast->id,next_terminal);
			next_terminal=0;
			break;
		
		case GEN_CAST+I+1:
		case GEN_CAST+I+2:
		case GEN_CAST+I+4:
		case GEN_CAST+I+8:
		case GEN_CAST+P+2:
		case GEN_CAST+P+4:
		case GEN_CAST+P+8:
			if(GEN_DEBUG)
				fprintf(stderr,"gen cast\n");
			gen_expression(ast->child[0]);
			resolve_terminal();
			interface->cast_expression(ast->id&0x1f,((struct constnode *)ast->child[1])->val&0x1f);
			break;
			
			
		case GEN_BAND+I+2:
		case GEN_BAND+I+4:
		case GEN_BAND+I+8:
		case GEN_MUL+I+1 :
		case GEN_MUL+I+2 :
		case GEN_MUL+I+4 :
		case GEN_MUL+I+8 :
			if(GEN_DEBUG)
				fprintf(stderr,"gen * / &\n");
		case GEN_ADD+I+2 :
		case GEN_ADD+I+4 :
		case GEN_ADD+I+8 :
		case GEN_ADD+P+2 :
		case GEN_ADD+P+4 :
		case GEN_ADD+P+8 :
			if(GEN_DEBUG)
				fprintf(stderr,"gen + / * / &\n");
			if(is_terminal(ast->child[0]))
			{
				if(GEN_DEBUG)
					fprintf(stderr,"\tgen left terminal\n");
				gen_expression(ast->child[1]);
				resolve_terminal();
				gen_expression(ast->child[0]);
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			else if(is_terminal(ast->child[1]))
			{
				if(GEN_DEBUG)
					fprintf(stderr,"\tgen right terminal\n");
				gen_expression(ast->child[0]);
				resolve_terminal();
				gen_expression(ast->child[1]);
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			else 
			{
				if(GEN_DEBUG)
					fprintf(stderr,"\tgen no terminal\n");
				gen_expression(ast->child[1]);
				resolve_terminal();
				interface->save_top();
				gen_expression(ast->child[0]);
				resolve_terminal();
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			break;
		case GEN_DIV+I+2 :
		case GEN_DIV+I+4 :
		case GEN_DIV+I+8 :
		case GEN_SUB+I+2 :	
		case GEN_SUB+I+4 :
		case GEN_SUB+I+8 :
		case GEN_SUB+P+2 :
		case GEN_SUB+P+4 :
		case GEN_SUB+P+8 :
			if(GEN_DEBUG)
				fprintf(stderr,"gen - / /\n");
			if(is_terminal(ast->child[1]))
			{
				gen_expression(ast->child[0]);
				resolve_terminal();
				gen_expression(ast->child[1]);
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			else 
			{
				gen_expression(ast->child[1]);
				resolve_terminal();
				interface->save_top();
				gen_expression(ast->child[0]);
				resolve_terminal();
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			break;
		case GEN_EQ+I+2 :
		case GEN_EQ+I+4 :
		case GEN_EQ+I+8 :
		case GEN_NE+I+2 :
		case GEN_NE+I+4 :
		case GEN_NE+I+8 :
		case GEN_LT+I+2 :
		case GEN_LT+I+4 :
		case GEN_LT+I+8 :
		case GEN_GT+I+2 :
		case GEN_GT+I+4 :
		case GEN_GT+I+8 :
		case GEN_LE+I+2 :
		case GEN_LE+I+4 :
		case GEN_LE+I+8 :
		case GEN_GE+I+2 :
		case GEN_GE+I+4 :
		case GEN_GE+I+8 :
			if(GEN_DEBUG)
				fprintf(stderr,"gen ==/!=/>/</>=/<= \n");
			if(is_terminal(ast->child[1]))
			{
				gen_expression(ast->child[0]);
				resolve_terminal();
				gen_expression(ast->child[1]);
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			else 
			{
				gen_expression(ast->child[1]);
				resolve_terminal();
				interface->save_top();
				gen_expression(ast->child[0]);
				resolve_terminal();
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			break;
		case GEN_LAND+I+2 :
		case GEN_LAND+I+4 :
		case GEN_LAND+I+8 :
			{
				int label=global_label_count;
				global_label_count=global_label_count+3;
				
				gen_expression(ast->child[0]);
				resolve_terminal();
				interface->jump_z(label);
				
				gen_expression(ast->child[1]);
				resolve_terminal();
				interface->jump_nz(label+1);
				
				interface->label(label);
				interface->load_const(0);
				interface->jump(label+2);
				
				interface->label(label+1);
				interface->load_const(1);
				
				interface->label(label+2);
			}
			break;
		case GEN_LOR+I+2 :
		case GEN_LOR+I+4 :
		case GEN_LOR+I+8 :
			{
				int label=global_label_count;
				global_label_count=global_label_count+3;
				
				gen_expression(ast->child[0]);
				resolve_terminal();
				interface->jump_nz(label);
				
				gen_expression(ast->child[1]);
				resolve_terminal();
				interface->jump_z(label+1);
				
				interface->label(label);
				interface->load_const(1);
				interface->jump(label+2);
				
				interface->label(label+1);
				interface->load_const(0);
				
				interface->label(label+2);
			}
			break;
		case GEN_ASSIGN+I+1 :
		case GEN_ASSIGN+I+2 :
		case GEN_ASSIGN+I+4 :
		case GEN_ASSIGN+I+8 :
		case GEN_ASSIGN+P+2 :
		case GEN_ASSIGN+P+4 :
		case GEN_ASSIGN+P+8 :
		{
			//int lvalue_id=((struct astnode *)ast->child[0])->id;
			if(GEN_DEBUG)
				fprintf(stderr,"gen =\n");
			if(is_address(ast->child[0]))
			{
				gen_expression(ast->child[1]);
				resolve_terminal();
				gen_expression(ast->child[0]);
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			else 
			{
				gen_expression(ast->child[1]);
				resolve_terminal();
				interface->save_top();
				gen_expression(ast->child[0]);
				resolve_terminal();
				interface->expression(ast->id,next_terminal);
				next_terminal=0;
			}
			break;
		}
		case GEN_SEL+I+2:
		case GEN_SEL+I+4:
		case GEN_SEL+I+8:
		case GEN_SEL+P+2:
		case GEN_SEL+P+4:
		case GEN_SEL+P+8:
		{
			int label=global_label_count;
			global_label_count=global_label_count+2;
			if(GEN_DEBUG)
				fprintf(stderr,"gen ? : \n");
			gen_expression(ast->child[0]);
			resolve_terminal();
			interface->jump_z(label);
			
			gen_expression(ast->child[1]);
			resolve_terminal();
			interface->jump(label+1);
			
			interface->label(label);
			gen_expression(ast->child[2]);
			resolve_terminal();
			
			interface->label(label+1);
		}
		break;
		case GEN_CALL:
		case GEN_CALL+I+1:
		case GEN_CALL+I+2:
		case GEN_CALL+I+4:
		case GEN_CALL+I+8:
		case GEN_CALL+P+2:
		case GEN_CALL+P+4:
		case GEN_CALL+P+8:
			interface->call(ast->child[0],ast->child[1],ast->id&0x7f,0);
			break;
			
		case GEN_VCALL:
		case GEN_VCALL+I+1:
		case GEN_VCALL+I+2:
		case GEN_VCALL+I+4:
		case GEN_VCALL+I+8:
		case GEN_VCALL+P+2:
		case GEN_VCALL+P+4:
		case GEN_VCALL+P+8:
			interface->call(ast->child[0],ast->child[1],ast->id&0x7f,1);
			break;
			
		default:
			fprintf(stderr,"Error, unknown expression type %d on line %d\n",ast->id,ast->line);
			break;
	}
	return 0;
}

int resolve_terminal()
{
	if(GEN_DEBUG)
		fprintf(stderr,"resolve terminal\n");
	if(next_terminal!=0)
		interface->terminal_expression(next_terminal);
	next_terminal=0;
	return 0;
}

int gen_strings(struct string_list *list)
{
	if(list==NULL)
		return 0;
	gen_strings(list->next);
	interface->string(list->n,list->str);
	return 0;
}
