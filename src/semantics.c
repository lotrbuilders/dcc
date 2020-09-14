#include <stdlib.h>
#include <stdio.h>
#include "errorhandling.h"
#include "symbols.h"
#include "type.h"
#include "node.h"
#include "identifier.h"
#include "struct.h"
#include "switch.h"

void *constant_evaluation(struct astnode *ast);

struct typelist *check_declaration(struct astnode *ast,char **name,struct astnode **initialization);
void check_specifier_list(struct typelist *type, int line);
struct astnode *check_constant_expression(struct astnode *ast);
struct astnode *check_parameter_list(struct astnode *ast);
struct astnode *check_statements(struct astnode *ast);
struct astnode *check_expression(struct astnode *ast);
struct typelist *check_expression_type(int op,struct astnode *left,struct astnode *right,struct astnode *extra,int line);
struct astnode *check_lvalue(struct astnode *ast);

struct typelist *current_function_type;

int DEBUG_SEMANTICS=1;

struct astnode *check_semantics(struct astnode *ast)
{
	if(DEBUG_SEMANTICS)
		fprintf(stderr,"Check semantics\n");
	if(ast==NULL)
		return ast;
	switch(ast->id)
	{
		case SYM_TRANSLATION_UNIT:
		for(struct astlist *list=(void*)ast;list!=0;list=list->next)
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check translation unit\n");
			check_semantics(list->child);
		}
		remove_globals();
		break;
			
		case SYM_TYPE_DECLARATION:
		{
			check_specifier_list(ast->child[0],ast->line);
			if(((struct typelist *)ast->child[0])->type!=TYPE_STRUCT)
				error("Expected a struct for type declaration",ast->line);
			else if(((struct typelist *)ast->child[0])->val==NULL)
				error("Expected a struct declaration for type declaration",ast->line);
			check_declaration_type(ast->child[0],ast->line);
		}
		break;
			
		case SYM_GLOBAL_DECLARATION:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check global declaration\n");
			struct typelist *list;
			ast->type=check_declaration(ast->child[1],(char**)&ast->child[0],(struct astnode **)&ast->child[2]);
			
			if(ast->type->type==TYPE_ARRAY)
			{
				if(ast->child[2]!=NULL)
					error("Initialization of array's not currently supported",ast->line);
			}
			else if((ast->type->type==TYPE_STRUCT)&&(ast->child[2]!=NULL))
			{
				error("Initialization of array currently unsupported",ast->line);
			}
			
			if(ast->type->type==TYPE_FUNCTION)
			{
				if(ast->child[2]!=NULL)
					error("Did not expect function declaration to have initialization",ast->line);
				else if(!is_returnable_type(ast->type->next))
				{
					error("Return type of function not returnable",ast->line);
				}
				
				check_parameter_list(ast->type->val);
				if(is_global(ast->child[0],&list))
				{
					if(compare_type(ast->type,list))
						error("Function type does not equal previous declaration",ast->line);
					else
					{
						struct astnode *list1=ast->type->val;
						struct astnode *list2=list->val;
						while((list1!=NULL)&&(list2!=NULL))
						{
							if(compare_type(list1->type,list2->type))
								error("Function argument does not equal previous declaration",ast->line);
							list1=list1->child[2];
							list2=list2->child[2];
						}
						if((list1==NULL)&&(list2!=NULL))
							error("Function argument does not equal previous declaration",ast->line);
						else if((list1!=NULL)&&(list2==NULL))
							error("Function argument does not equal previous declaration",ast->line);
					}
				}
				else
					add_global_id(ast->child[0],ast->type,0);
			}
			else 
			{
				if(ast->type->type==TYPE_VOID)
					error("Non-function global cannot have type void",ast->line);
				if(is_global(ast->child[0],&list))//if already declared 
				{
					if(compare_type(ast->type,list))
						error("Function type does not equal previous declaration",ast->line);
					else if(is_defined(ast->child[0])&&(ast->child[2]!=NULL))
						error("Variable already defined",ast->line);
					else if(ast->child[2]!=NULL)
					{
						define_global(ast->child[0],1);
						check_constant_expression(ast->child[2]);
						check_expression(ast->child[2]);
						ast->child[2]=constant_evaluation(ast->child[2]);
					}
				}
				else
				{
					add_global_id(ast->child[0],ast->type,ast->child[2]!=NULL);
					if(ast->child[2]!=NULL)
					{
						check_constant_expression(ast->child[2]);
						check_expression(ast->child[2]);
						ast->child[2]=constant_evaluation(ast->child[2]);
					}
				}
			}
			
		}
		break;
		
		case SYM_FUNCTION_DEFENITION:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check function defenition\n");
			struct astnode *init_pointer;
			ast->type=check_declaration(ast->child[1],(char**)&ast->child[0],(struct astnode **)&init_pointer);
			struct typelist *type=ast->type;
			current_function_type=ast->type->next;
			if(type->type!=TYPE_FUNCTION)
			{
				error("Expected function type for function declaration",ast->line);
			}
			else if(init_pointer!=NULL)
			{
				error("Did not expect a function defenition to include an expression initialization",ast->line);
			}
			else if(!is_returnable_type(type->next))
			{
				error("Return type of function not returnable",ast->line);
			}
			else 
			{
				struct typelist *list;
				check_parameter_list(ast->type->val);
				
				if(is_global(ast->child[0],&list))
				{
					if(is_defined(ast->child[0]))
						error("Function declared multiple times",ast->line);
					else 
						define_global(ast->child[0],1);
					
					if(compare_type(ast->type,list))
						error("Function type does not equal previous declaration",ast->line);
					else
					{
						struct astnode *list1=ast->type->val;
						struct astnode *list2=list->val;
						while((list1!=NULL)&&(list2!=NULL))
						{
							if(compare_type(list1->type,list2->type))
								error("Function argument does not equal previous declaration",ast->line);
							list1=list1->child[2];
							list2=list2->child[2];
						}
						if((list1==NULL)&&(list2!=NULL))
							error("Function argument does not equal previous declaration",ast->line);
						else if((list1!=NULL)&&(list2==NULL))
							error("Function argument does not equal previous declaration",ast->line);
					}
				}
				else
					add_global_id(ast->child[0],ast->type,1);
				
				for(struct astnode *list=ast->type->val;list!=NULL;list=list->child[2])
				{
					if(list->type->type==TYPE_ARRAY)
						list->type->type=TYPE_POINTER;
					add_argument_id(list->child[0],list->type);
				}
				
				ast->child[2]=check_statements(ast->child[2]);
				remove_arguments();
			}
		}
		break;
		
	}
	return ast;
}

void check_specifier_list(struct typelist *type, int line)
{
	if(DEBUG_SEMANTICS)
			fprintf(stderr,"Check specifier list\n");
	if(type==NULL)
		error("Expected specfifier list",line);
	else if(type->type==TYPE_INT)
	{
		if(type->next!=NULL)
			error("Unexpected type after type int",line);
	}
	else if(type->type==TYPE_CHAR)
	{
		if(type->next!=NULL)
			error("Unexpected type after type char",line);
	}
	else if(type->type==TYPE_SHORT)
	{
		if((type->next!=NULL))
		{
			if(type->next->type==TYPE_INT)
			{
				free(type->next);
				type->next=NULL;
			}
			else 
				error("Unexpected type after type char",line);
		}
	}
	else if(type->type==TYPE_LONG)
	{
		if((type->next!=NULL))
		{
			if(type->next->type==TYPE_INT)
			{
				free(type->next);
				type->next=NULL;
			}
			else 
				error("Unexpected type after type char",line);
		}
	}
	else if(type->type==TYPE_STRUCT)
	{
		if(type->next!=NULL)
			error("Unexpected type after type struct",line);
	}
	else if(type->type==TYPE_VOID)
	{
		if(type->next!=NULL)
			error("Unexpected type after type struct",line);
	}
	else 
		error("Unexpected type specfifier",line);
	return;
}

struct typelist *check_declaration(struct astnode *ast,char **name,struct astnode **initialization)
{
	if(DEBUG_SEMANTICS)
		fprintf(stderr,"Check declaration\n");
	if(ast==NULL)
	{
		fprintf(stderr,"check_declaration ast==NULL\n");
		exit(-1);
	}
	if(ast->id!=SYM_DECLARATION)
	{
		error("Expected decleration when checking for decleration",ast->line);
		exit(-1);
	}
	
	struct astnode *init_declarator=ast->child[1];
	if(init_declarator->id!=SYM_INIT_DECLARATOR)
	{
		error("Expected init declarator when checking for decleration",ast->line);
		exit(-1);
	}
	if(init_declarator->child_count==1)
		*initialization=NULL;
	else 
		*initialization=init_declarator->child[1];
	
	check_specifier_list(ast->child[0],ast->line);
	*name=find_type_identifier(init_declarator->child[0],ast->line);
	struct typelist *type=replace_typelist(init_declarator->child[0],ast->child[0],TYPE_ID);
	check_declaration_type(type,ast->line);
	return type;
}

struct astnode *check_parameter_list(struct astnode *ast)
{
	if(DEBUG_SEMANTICS)
		fprintf(stderr,"Check parameter list\n");
	for(struct astnode *list=ast;list!=NULL;list=list->child[2])
	{
		struct astnode *init;
		list->type=check_declaration(list->child[1],(void*)&list->child[0],&init);
		if(init!=NULL)
			error("Did not expect initialization in decleration",list->line);
		if(list->type->type==TYPE_FUNCTION)
			error("Function is not of passable type",list->line);
		if(list->type->type==TYPE_VOID)
			error("Void is not of passable type",list->line);
		if(DEBUG_SEMANTICS)
			fprintf(stderr,"check parameter %s\n",(char*)list->child[0]);
	}
	return ast;
}

struct typelist *check_type_name(struct astnode *ast)
{
	if(ast==NULL)
	{
		return new_type(TYPE_INT,NULL,NULL);
	}
	if(ast->id!=SYM_TYPE_NAME)
	{
		error("Expected decleration when checking for decleration",ast->line);
		exit(-1);
	}
	check_specifier_list(ast->child[0],ast->line);
	struct typelist *type=append_typelist(ast->child[1],ast->child[0]);
	check_declaration_type(type,ast->line);
	return type;
}

int in_loop;

struct astnode *check_statements(struct astnode *ast)
{
	if(DEBUG_SEMANTICS)
		fprintf(stderr,"Check statements\n");
	if(ast==NULL)
		return ast;
	switch(ast->id)
	{
		case SYM_COMPOUND_STATEMENT:
			enter_block();
			check_statements(ast->child[0]);
			ast->child[1]=newconst(SYM_NUM,ast->line,1);//count statement list;
			ast->child[2]=newconst(SYM_NUM,ast->line,leave_block());//count var space;
			break;
		case SYM_BLOCK_ITEM_LIST:
		{
			struct astlist *node=(void*)ast;
			check_statements(node->child);
			check_statements(node->next);
			break;
		}
		case SYM_RETURN:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check return\n");
			if(ast->child[0]==NULL)
			{
				if(current_function_type->type!=TYPE_VOID)
					error("Return without expression in non_void function",ast->line);
			}
			else 
			{
				check_expression(ast->child[0]);
				if(is_compatible_type(((struct astnode *)ast->child[0])->type,current_function_type))
					error("Return with incompatible return type",ast->line);
			}
			break;
		case SYM_IF_ELSE:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"if else\n");
			check_statements(ast->child[2]);
		case SYM_IF:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"if\n");
			check_expression(ast->child[0]);
			check_statements(ast->child[1]);
			break;
		case SYM_WHILE:
		case SYM_DO_WHILE:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"while\n");
			check_expression(ast->child[0]);
			
			int old_loop=in_loop;
			in_loop=ast->id;
			check_statements(ast->child[1]);
			
			in_loop=old_loop;
			break;
		}
		case SYM_FOR:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"while\n");
			enter_block();
			
			check_statements(ast->child[0]);
			if(ast->child[1]==NULL)
				ast->child[1]=newconst(SYM_NUM,ast->line,1);
			else
				check_expression(ast->child[1]);
			check_expression(ast->child[2]);
			
			int old_loop=in_loop;
			in_loop=ast->id;
			
			check_statements(ast->child[3]);
			
			in_loop=old_loop;

			leave_block();
			break;
		}
		case SYM_BREAK:
		{
			if((in_loop==0))
			{
				error("Expected break to be in a loop or switch statement",ast->line);
			}
			else if(in_loop==SYM_SWITCH)
				ast->id=SYM_SWITCH_BREAK;
			break;
		}
		case SYM_CONTINUE:
		{
			if((in_loop==0)&&(in_loop!=SYM_SWITCH))
			{
				error("Expected continue to be in a loop",ast->line);
			}
			break;
		}
		case SYM_SWITCH:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check switch\n");
			int old_loop=in_loop;
			in_loop=ast->id;
			
			check_expression(ast->child[0]);
			struct typelist *type=((struct astnode *)ast->child[0])->type;
			if(!is_arithmatic_type(type))
				error("Expected switch expression to be of arithmatic type",ast->line);
			enter_switch(ast->line);
			check_statements(ast->child[1]);
			ast->child[2]=leave_switch();
			
			in_loop=old_loop;
			break;
		}
		case SYM_CASE:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check case\n");
			if(!in_switch)
				error("Expected case to be in a switch statement",ast->line);
			else 
			{
				check_expression(ast->child[0]);
				check_constant_expression(ast->child[0]);
				ast->child[0]=constant_evaluation(ast->child[0]);
				struct constnode *val=ast->child[0];
				if(DEBUG_SEMANTICS)
					fprintf(stderr,"Checked case\n");
				if(!is_arithmatic_type(val->type))
					error("Expected case expression to be of arithmatic type",ast->line);
				if(val->id!=SYM_NUM)
					error("Expected case expression to be constant",ast->line);
				else
				{
					ast->child[2]=newconst(SYM_NUM,ast->line,add_case(val->val));
				}
			}
			check_statements(ast->child[1]);
			break;
		}
		case SYM_DEFAULT:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check default\n");
			if(!in_switch)
				error("Expected case to be in a switch statement",ast->line);
			else 
				ast->child[2]=newconst(SYM_NUM,ast->line,add_default());
			check_statements(ast->child[1]);
			break;
		}
		case SYM_LOCAL_DECLARATION:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check local declaration\n");
			ast->type=check_declaration(ast->child[1],(char**)&ast->child[0],(struct astnode **)&ast->child[2]);
			if(id_inblock(ast->child[0]))
			{
				error("Identifier already defined",ast->line);
			}
			else if(ast->type->type==TYPE_FUNCTION)
			{
				error("Local function declerations are not supported",ast->line);
			}
			else if((ast->type->type==TYPE_ARRAY)&&(ast->child[2]!=NULL))
			{
				error("Initialization of array currently unsupported",ast->line);
			}
			else if((ast->type->type==TYPE_STRUCT)&&(ast->child[2]!=NULL))
			{
				error("Initialization of struct currently unsupported",ast->line);
			}
			else if(ast->type->type==TYPE_VOID)
			{
				error("Local variable cannot have type void",ast->line);
			}
			else
			{
				add_local_id(ast->child[0],ast->type);
			}
			
			if(ast->child[2]!=NULL)
			{
				check_expression(ast->child[2]);
				if(is_compatible_type(((struct astnode *)ast->child[2])->type,ast->type))
				{
					print_type(((struct astnode *)ast->child[2])->type);
					print_type(ast->type);
					error("Type does not match declaration type",ast->line);
				}
			}
			break;
		}
		default:
			check_expression(ast);
	}
	return ast;
}

struct astnode *check_expression(struct astnode *ast)
{
	if(DEBUG_SEMANTICS)
		fprintf(stderr,"Check expression\n");
	if(ast==NULL)
		return ast;
	switch(ast->id)
	{
		case SYM_NUM:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check number\n");
			ast->type=new_type(TYPE_INT,NULL,NULL);
			break;
		case SYM_ID:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check id\n");
			int loc=find_local_id(ast->child[0],(void *)&ast->type);
			if(loc==-1)
			{
				int loc=find_argument_id(ast->child[0],(void *)&ast->type);
				if(loc==-1)
				{
					if(is_global(ast->child[0],(void *)&ast->type))
					{
						ast->id=SYM_IDG;
					}
					else
						error("Identifier not defined",ast->line);
				}
				else 
					ast->id=SYM_IDF;
			}
			else
				ast->id=SYM_IDL;
			print_type(ast->type);
			if(ast->type->type==TYPE_FUNCTION)
			{
				struct astnode *node=newnode(ast->id,ast->line,1);
				node->child[0]=ast->child[0];
				node->type=ast->type;
				ast->id=SYM_ADDRESS;
				ast->child[0]=node;
				ast->type=new_type(TYPE_POINTER,NULL,ast->type);
			}
			break;
		}
		case SYM_STRING:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check string\n");
			ast->type=new_type(TYPE_POINTER,NULL,new_type(TYPE_CHAR,NULL,NULL));
			break;
		case SYM_SIZEOF:
			ast->child[1]=check_type_name(ast->child[0]);
			print_type(ast->child[1]);
			switch(((struct typelist *)ast->child[1])->type)
			{
				case TYPE_FUNCTION:error("Unexpected sizeof function",ast->line);break;
				case TYPE_VOID:error("Unexpected sizeof void",ast->line);break;
			}
			ast->type=new_type(TYPE_INT,NULL,NULL);
			break;
		case SYM_FUNCTION_CALL:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check function call\n");
			struct astnode *child=ast->child[0];
			
			check_lvalue(child);
			if(child->type->type!=TYPE_FUNCTION)
				ast->id=SYM_VFUNCTION_CALL;
			
			struct typelist *type=child->type;
			for(struct astlist *list=ast->child[1];list!=NULL;list=list->next)
			{
				check_expression(list->child);
			}
			
			if(type->type==TYPE_FUNCTION)
			{
				ast->type=type->next;
			}
			else if((type->type==TYPE_POINTER)&&(type->next!=NULL)&&(type->next->type==TYPE_FUNCTION))
			{
				ast->type=type->next->next;
			}
			else
			{
				error("Expected call to function",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
			}
			
			break;
		}
		case SYM_ADDRESS:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check address\n");
			check_lvalue(ast->child[0]);
			ast->type=check_expression_type(ast->id,ast->child[0],NULL,NULL,ast->line);
			break;
		case SYM_INDIRECT:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check dereference\n");
			check_expression(ast->child[0]);
			ast->type=check_expression_type(ast->id,ast->child[0],NULL,NULL,ast->line);
			break;
		case SYM_ARRAY:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check array dereference\n");
			check_expression(ast->child[0]);
			check_expression(ast->child[1]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],NULL,ast->line);
			break;
		case SYM_DOT:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check member acces\n");
			check_expression(ast->child[0]);
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type;
			int location;
			if(struct_type->type!=TYPE_STRUCT)
			{
				error("Expected member acces to access struct",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			ast->type=find_struct_member(struct_type->structname,ast->child[1],&location);
			if(ast->type==NULL)
			{
				error("Expected member access to access valid member",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			break;
		}
		case SYM_ARROW:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check member access through pointer\n");
			check_expression(ast->child[0]);
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type;
			int location;
			if(struct_type->type!=TYPE_POINTER)
			{
				error("Expected pointer member acces to access via pointer",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			if(struct_type->next->type!=TYPE_STRUCT)
			{
				error("Expected member acces to access struct",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			ast->type=find_struct_member(struct_type->next->structname,ast->child[1],&location);
			if(ast->type==NULL)
			{
				error("Expected member access to access valid member",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			break;
		}
		case '~':
		case '!':
		case SYM_NEGATE:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check ~/!/-\n");
			check_expression(ast->child[0]);
			ast->type=check_expression_type(ast->id,ast->child[0],NULL,NULL,ast->line);
			break;
		case SYM_CAST:
			check_expression(ast->child[0]);
			ast->type=check_type_name(ast->child[1]);
			if(ast->type->type==TYPE_FUNCTION)
				error("Unexpected cast to function",ast->line);
			else if(ast->type->type==TYPE_STRUCT)
				error("Unexpected cast to struct",ast->line);
			break;
		case '+':
		case '-':
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check +/-\n");
			check_expression(ast->child[0]);
			check_expression(ast->child[1]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],NULL,ast->line);
			break;
		case '*':
		case '/':
		case SYM_LAND:
		case SYM_LOR:
		case '&':
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check */||/&&/&\n");
			check_expression(ast->child[0]);
			check_expression(ast->child[1]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],NULL,ast->line);
			break;
		case SYM_EQ:
		case SYM_NE:
		case '>':
		case '<':
		case SYM_LE:
		case SYM_GE:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check ==/!=/</>/<=/>=\n");
			check_expression(ast->child[0]);
			check_expression(ast->child[1]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],NULL,ast->line);
			break;
		case SYM_SEL:
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check ? : \n");
			check_expression(ast->child[0]);
			check_expression(ast->child[1]);
			check_expression(ast->child[2]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],ast->child[2],ast->line);
			break;
		case '=':
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check =\n");
			check_lvalue(ast->child[0]);
			check_expression(ast->child[1]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],NULL,ast->line);
			break;
		default:
			error("Unkown symbol whilst checking statements",ast->line);
	}
	return ast;
}



struct astnode *check_lvalue(struct astnode *ast)
{
	if(ast==NULL)
		return ast;
	switch(ast->id)
	{
		case SYM_ID:
		{
			int loc=find_local_id(ast->child[0],(void *)&ast->type);
			if(loc==-1)
			{
				int loc=find_argument_id(ast->child[0],(void *)&ast->type);
				if(loc==-1)
				{
					if(is_global(ast->child[0],(void *)&ast->type))
					{
						ast->id=SYM_IDG;
					}
					else
						error("Identifier not defined",ast->line);
				}
				else 
					ast->id=SYM_IDF;
			}
			else
				ast->id=SYM_IDL;
			break;
		}
		case SYM_INDIRECT:
		{
			check_expression(ast->child[0]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],NULL,ast->line);
			break;
		}
		case SYM_ARRAY:
		{
			check_expression(ast->child[0]);
			check_expression(ast->child[1]);
			ast->type=check_expression_type(ast->id,ast->child[0],ast->child[1],NULL,ast->line);
			break;
		}
		case SYM_DOT:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check member acces\n");
			check_expression(ast->child[0]);
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type;
			int location;
			if(struct_type->type!=TYPE_STRUCT)
			{
				error("Expected member acces to access struct",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			ast->type=find_struct_member(struct_type->structname,ast->child[1],&location);
			if(ast->type==NULL)
			{
				error("Expected member access to access valid member",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			break;
		}
		case SYM_ARROW:
		{
			if(DEBUG_SEMANTICS)
				fprintf(stderr,"Check member access through pointer\n");
			check_expression(ast->child[0]);
			struct typelist *struct_type=((struct astnode *)ast->child[0])->type;
			int location;
			if(struct_type->type!=TYPE_POINTER)
			{
				error("Expected pointer member acces to access via pointer",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			if(struct_type->next->type!=TYPE_STRUCT)
			{
				error("Expected member acces to access struct",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			ast->type=find_struct_member(struct_type->next->structname,ast->child[1],&location);
			if(ast->type==NULL)
			{
				error("Expected member access to access valid member",ast->line);
				ast->type=new_type(TYPE_INT,NULL,NULL);
				break;
			}
			break;
		}
		default:
			error("Bad lvalue",ast->line);
	}
	return ast;
}



struct typelist *check_expression_type(int op,struct astnode *left,struct astnode *right,struct astnode *extra,int line)
{
	struct typelist *left_type=NULL;
	struct typelist *right_type=NULL;
	struct typelist *extra_type=NULL;
	if(left!=NULL)
		left_type=unary_type_promotion(left->type);
	if(right!=NULL)
		right_type=unary_type_promotion(right->type);
	if(extra!=NULL)
		extra_type=unary_type_promotion(extra->type);
	if(DEBUG_SEMANTICS)
		fprintf(stderr,"Check expression type\n");
	switch(op)
	{
		case SYM_ADDRESS:
			return new_type(TYPE_POINTER,NULL,left->type);
		case SYM_INDIRECT:
			if((left_type->type!=TYPE_POINTER)&&(left_type->type!=TYPE_ARRAY))
			{
				error("Dereferencing non pointer",line);
				return new_type(TYPE_INT,NULL,NULL);
			}
			else if(left_type->next->type==TYPE_VOID)
			{
				error("Dereferencing void pointer",line);
				return new_type(TYPE_INT,NULL,NULL);
			}
			else 
				return left_type->next;
		case SYM_ARRAY:
			if(!is_arithmatic_type(right_type))
				error("Array index is not of arithmatic type",line);
			if((left_type->type!=TYPE_POINTER)&&(left_type->type!=TYPE_ARRAY))
			{
				error("Dereferencing non pointer",line);
				return new_type(TYPE_INT,NULL,NULL);
			}
			else if(left_type->next->type==TYPE_VOID)
			{
				error("Dereferencing void pointer",line);
				return new_type(TYPE_INT,NULL,NULL);
			}
			else 
				return left_type->next;
		case '~':
		case '!':
		case SYM_NEGATE:
			if(!is_arithmatic_type(left_type))
				error("Unsupported unary operation on non arithmatic type",line);
			return left_type;
		case '+':
		case '-':
			if((left_type->type==TYPE_POINTER)&&(right_type->type==TYPE_POINTER))
				error("Unexpected addition of two pointers",line);
			if(left_type->type==TYPE_POINTER)
				return (left_type);
			else if(right_type->type==TYPE_POINTER)
				return (right_type);
			else if(!is_arithmatic_type(left_type)&&!is_arithmatic_type(right_type))
				error("Unsupported addition operation on non arithmatic type",line);
			else 
				return binary_type_promotion(left_type,right_type);
			break;
		case '*':
		case '/':
		case SYM_LAND:
		case SYM_LOR:
		case '&':
			if((!is_arithmatic_type(left_type))||(!is_arithmatic_type(right_type)))
				error("Unexpected operation on non arithmatic type",line);
			return binary_type_promotion(left_type,right_type);
			break;
		case SYM_EQ:
		case SYM_NE:
		case '>':
		case '<':
		case SYM_LE:
		case SYM_GE:
			if(is_compatible_type(left_type,right_type)!=0)
				error("Comparison between incompatible types",line);
			return new_type(TYPE_INT,NULL,NULL);
			break;
		case SYM_SEL:
			if(compare_type(extra_type,right_type)!=0)
				error("Left and right type in ? : do not match",line);
			return extra->type;
			break;
		case '='://Unary type promotion does not apply to assignment
			print_type(left->type);
			if(left->type->type==TYPE_ARRAY||left->type->type==TYPE_STRUCT)
				error("Can not assign to type array or struct",line);
			if(is_compatible_type(left->type,right->type)!=0)
				error("Assignment of incompatible types",line);
			return left->type;
			break;
		default:
			error("Unkown symbol whilst checking statements",line);
	}
	error("Bad expression type\n",line);
	return NULL;
}



struct astnode *check_constant_expression(struct astnode *ast)
{
	if(DEBUG_SEMANTICS)
		fprintf(stderr,"Check constant expression\n");
	if(ast==NULL)
		return ast;
	switch(ast->id)
	{
		case SYM_NUM:
		case SYM_SIZEOF:
			break;
		case SYM_ID:
		case SYM_IDL:
		case SYM_IDF:
		case SYM_IDG:
			error("Identifier not allowed in constant declaration",ast->line);
			break;
		case SYM_FUNCTION_CALL:
		case SYM_VFUNCTION_CALL:
			error("Function call not allowed in constant declaration",ast->line);
			break;
		case SYM_ADDRESS:
			error("Address not allowed in constant declaration",ast->line);
			break;
		case SYM_INDIRECT:
			error("Pointer indirection not allowed in constant declaration",ast->line);
			break;
		case SYM_STRING:
			error("String not implemented in constant declaration",ast->line);
			break;
		case SYM_CAST: 
		case '~':
		case '!':
		case SYM_NEGATE:
			check_constant_expression(ast->child[0]);
			break;
		case '+':
		case '&':
		case '-':
		case '*':
		case '/':
		case SYM_LAND:
		case SYM_LOR:
		case SYM_EQ:
		case SYM_NE:
		case '>':
		case '<':
		case SYM_LE:
		case SYM_GE:
			check_constant_expression(ast->child[0]);
			check_constant_expression(ast->child[1]);
			break;
		case SYM_SEL:
			error("conditional-expression not allowed in constant declaration",ast->line);
			break;
		case '=':
			error("assignment not allowed in constant declaration",ast->line);
			break;
		default:
			error("Unkown symbol whilst checking constant expression",ast->line);
	}
	return ast;
}
