/*
	Declaration parser DCC
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
#include <stddef.h>
#include <stdio.h>
#include "tokens.h"
#include "lexer.h"
#include "errorhandling.h"
#include "type.h"
#include "node.h"
#include "symbols.h"

char *strdup(char *s);
int peek();
int pop();

struct astnode *parse_expression();

struct astnode *parse_declaration();
struct typelist *parse_declaration_specifiers();
struct astnode *parse_init_declarator();
struct typelist *parse_declarator();
struct typelist *parse_direct_declarator();
struct astnode *parse_parameter_list();
struct astlist *parse_struct_declaration_list();

struct astnode *parse_type_name();
struct typelist *parse_abstract_declarator();
struct typelist *parse_direct_abstract_declarator();

int PARSER_DEBUG;

struct astnode *parse_declaration()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse declaration\n");
	struct typelist *specifiers=parse_declaration_specifiers();
	if(peek()!=';')
	{
		struct astnode *ptr=newnode(SYM_DECLARATION,line_number,2);
		ptr->child[0]=specifiers;
		ptr->child[1]=parse_init_declarator();
		return ptr;
	}
	struct astnode *ptr=newnode(SYM_TYPE_DECLARATION,line_number,1);
	ptr->child[0]=specifiers;
	return ptr;
}

struct typelist *parse_declaration_specifiers()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse declaration specifier\n");
	if(peek()==TOK_INT)
	{
		pop();
		struct typelist *type=new_type(TYPE_INT,NULL,parse_declaration_specifiers());
		return type;
	}
	else if(peek()==TOK_CHAR)
	{
		pop();
		struct typelist *type=new_type(TYPE_CHAR,NULL,parse_declaration_specifiers());
		return type;
	}
	else if(peek()==TOK_SHORT)
	{
		pop();
		struct typelist *type=new_type(TYPE_SHORT,NULL,parse_declaration_specifiers());
		return type;
	}
	else if(peek()==TOK_LONG)
	{
		pop();
		struct typelist *type=new_type(TYPE_LONG,NULL,parse_declaration_specifiers());
		return type;
	}
	else if(peek()==TOK_VOID)
	{
		pop();
		struct typelist *type=new_type(TYPE_VOID,NULL,parse_declaration_specifiers());
		return type;
	}
	else if(peek()==TOK_STRUCT)
	{
		pop();
		if(peek()!=TOK_ID)
		{
			parser_error("Expected identifier after 'struct' not '%T'",pop(),line_number);
			return NULL;
		}
		pop();
		char *name=strdup(lex_buffer);
		struct astlist *decl_list=NULL;
		if(peek()=='{')
		{
			pop();
			decl_list=parse_struct_declaration_list();
			if(peek()!='}')
			{
				parser_error("Expected '}' not '%T'",pop(),line_number);
				return NULL;
			}
			pop();
			
		}
		struct typelist *type=new_type(TYPE_STRUCT,decl_list,parse_declaration_specifiers());
		type->structname=name;
		return type;
	}
	else 
	{
		return NULL;
	}
	
}

struct astnode *parse_init_declarator()
{
	struct typelist *declarator=parse_declarator();
	if(peek()=='=')
	{
		pop();
		struct astnode *node=newnode(SYM_INIT_DECLARATOR,line_number,2);
		node->child[0]=declarator;
		node->child[1]=parse_expression();
		return node;
	}
	else 
	{
		struct astnode *node=newnode(SYM_INIT_DECLARATOR,line_number,1);
		node->child[0]=declarator;
		return node;
	}
}

struct typelist *parse_declarator()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse declarator\n");
	if(peek()=='*')
	{
		pop();
		struct typelist *type=parse_declarator();
		type=insert_typelist(type,new_type(TYPE_POINTER,NULL,NULL));
		return type;
	}
	else 
		return parse_direct_declarator();
}

struct typelist *parse_direct_declarator()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse direct declarator\n");
	struct typelist *top=NULL;
	struct typelist *new;
	if(peek()==TOK_ID)
	{
		pop();
		top=new_type(TYPE_ID,strdup(lex_buffer),NULL);
	}
	else if(peek()=='(')
	{
		pop();
		top=parse_declarator();
		if(peek()!=')')
			parser_error("Expected ')' not %T",peek(),line_number);
		else pop();
	}
	for(int tk=peek();;tk=peek())
	{
		if(tk=='(')
		{
			pop();
			struct astnode *parameters=parse_parameter_list();
			if(peek()!=')')
				parser_error("Expected ')' not %T",pop(),line_number);
			pop();
			new=new_type(TYPE_FUNCTION,parameters,top);
			top=insert_typelist(top,new);
		}
		else if(tk=='[')
		{
			pop();
			struct astnode *exp=parse_expression();
			if(peek()!=']')
				parser_error("Expected ']' not %T",pop(),line_number);
			pop();
			new=new_type(TYPE_ARRAY,exp,top);
			top=insert_typelist(top,new);
		}
		else 
			break;
	}
	return top;
}

struct astnode *parse_parameter_list()
{
	if(peek()==')')
		return NULL;
	struct astnode *decl=parse_declaration();
	if(peek()==',')
		pop();
	struct astnode *node=newnode(SYM_PARAMETER_LIST,line_number,3);
	//node->child[0];//name
	node->child[1]=decl;
	node->child[2]=parse_parameter_list();
	return node;
}

struct astlist *parse_struct_declaration_list()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse struct declaration list\n");
	if(peek()=='}')
		return NULL;
	int line=line_number;
	struct astnode *decl=parse_declaration();
	if(peek()!=';')
	{
		parser_error("Expected ';' not %T",pop(),line_number);
		return NULL;
	}
	pop();
	return newastlist(SYM_STRUCT_DECLARATION_LIST,line,decl,parse_struct_declaration_list());
}

struct astnode *parse_type_name()
{
	struct typelist *specifier=parse_declaration_specifiers();
	struct typelist *decl=parse_abstract_declarator();
	struct astnode *node=newnode(SYM_TYPE_NAME,line_number,2);
	node->child[0]=specifier;
	node->child[1]=decl;
	return node;
}

struct typelist *parse_abstract_declarator()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse abstract declarator\n");
	if(peek()=='*')
	{
		pop();
		struct typelist *type=parse_abstract_declarator();
		if(PARSER_DEBUG)
			print_type(type);
		if(type==NULL)
			return new_type(TYPE_POINTER,NULL,NULL);
		struct typelist *list;
		for(list=type;(list->next!=NULL);list=list->next);
		struct typelist *temp=list->next;
		list->next=new_type(TYPE_POINTER,NULL,temp);
		//print_type(type);
		return type;
	}
	else 
		return parse_direct_abstract_declarator();
}

struct typelist *parse_direct_abstract_declarator()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse direct abstract declarator\n");
	struct typelist *top=NULL;
	struct typelist *new;
	if(peek()=='(')
	{
		pop();
		top=parse_declarator();
		if(peek()!=')')
			parser_error("Expected ')' not %T",peek(),line_number);
		else pop();
	}
	for(int tk=peek();;tk=peek())
	{
		if(tk=='(')
		{
			pop();
			if(peek()!=')')
			{
				parser_error("Expected ')' not %T",pop(),line_number);
				return NULL;
			}
			pop();
			new=new_type(TYPE_FUNCTION,NULL,top);
			top=new;
		}
		else if(tk=='[')
		{
			pop();
			struct astnode *val=parse_expression();
			if(peek()!=']')
			{
				parser_error("Expected '[' not %T",pop(),line_number);
				return NULL;
			}
			pop();
			new=new_type(TYPE_ARRAY,val,top);
			top=new;
		}
		else
			break;
	}
	return top;
}