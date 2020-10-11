/*
	Expression parser DCC
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
void push(int tk);

struct astnode *parse_type_name();

struct astnode *parse_assignment_expression();
struct astnode *parse_conditional_expression();
struct astnode *parse_logical_or_expression();
struct astnode *parse_logical_and_expression();
struct astnode *parse_and_expression();
struct astnode *parse_equality_expression();
struct astnode *parse_relational_expression();
struct astnode *parse_additive_expression();
struct astnode *parse_multiplicative_expression();
struct astnode *parse_cast_expression();
struct astnode *parse_unary_expression();
struct astnode *parse_postfix_expression();
struct astlist *parse_expression_list();
struct astnode *parse_primary_expression();

int PARSER_DEBUG;

struct astnode *parse_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing expression\n");
	return parse_assignment_expression();
}

struct astnode *parse_assignment_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing assignment expression\n");
	struct astnode *top;
	top=parse_conditional_expression();
	if(peek()=='=')
	{
		struct astnode *new=newnode(pop(),line_number,2);
		new->child[1]=parse_assignment_expression();
		new->child[0]=top;
		top=new;
	}
	return top;
}

struct astnode *parse_conditional_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing conditional expression\n");
	struct astnode *exp=parse_logical_or_expression();
	if(peek()=='?')
	{
		pop();
		int line=line_number;
		struct astnode *if_true=parse_expression();
		if(peek()!=':')
		{
			parser_error("Expected ':' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *if_else=parse_conditional_expression();
		struct astnode *node=newnode(SYM_SEL,line,3);
		node->child[0]=exp;
		node->child[1]=if_true;
		node->child[2]=if_else;
		return node;
	}
	else 
		return exp;
}

struct astnode *parse_logical_or_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing logical-or expression\n");
	struct astnode *top;
	top=parse_logical_and_expression();
	while(peek()==TOK_LOR)
	{
		pop();
		struct astnode *new=newnode(SYM_LOR,line_number,2);
		new->child[1]=parse_logical_and_expression();
		new->child[0]=top;
		top=new;
	}
	return top;
}

struct astnode *parse_logical_and_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing logical-and expression\n");
	struct astnode *top;
	top=parse_and_expression();
	while(peek()==TOK_LAND)
	{
		pop();
		struct astnode *new=newnode(SYM_LAND,line_number,2);
		new->child[1]=parse_and_expression();
		new->child[0]=top;
		top=new;
	}
	return top;
}

struct astnode *parse_and_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing and expression\n");
	struct astnode *top;
	top=parse_equality_expression();
	while(peek()=='&')
	{
		pop();
		struct astnode *new=newnode('&',line_number,2);
		new->child[1]=parse_equality_expression();
		new->child[0]=top;
		top=new;
	}
	return top;
}

struct astnode *parse_equality_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing equality expression\n");
	struct astnode *top;
	top=parse_relational_expression();
	while((peek()==TOK_EQ)||(peek()==TOK_NE))
	{
		struct astnode *new;
		if(peek()==TOK_EQ)
			new=newnode(SYM_EQ,line_number,3);
		else
			new=newnode(SYM_NE,line_number,3);
		pop();
		new->child[1]=parse_relational_expression();
		new->child[0]=top;
		top=new;
	}
	return top;
}

struct astnode *parse_relational_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing relational expression\n");
	struct astnode *top;
	top=parse_additive_expression();
	int pk=peek();
	while((pk=='<')||(pk=='>')||(pk==TOK_LE)||(pk==TOK_GE))
	{
		int id;
		pop();
		switch(pk)
		{
			case '<':
			case '>':	 id=pk;		break;
			case TOK_LE: id=SYM_LE;	break;
			case TOK_GE: id=SYM_GE;	break;
			default:	 id=pk;		break;
		}
		struct astnode *new=newnode(id,line_number,3);
		new->child[1]=parse_additive_expression();
		new->child[0]=top;
		top=new;
		pk=peek();
	}
	return top;
}

struct astnode *parse_additive_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing additive expression\n");
	struct astnode *top;
	top=parse_multiplicative_expression();
	while((peek()=='+'||peek()=='-'))
	{
		struct astnode *new=newnode(pop(),line_number,2);
		new->child[1]=parse_multiplicative_expression();
		new->child[0]=top;
		top=new;
	}
	return top;
}

struct astnode *parse_multiplicative_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing multiplicative expression\n");
	struct astnode *top;
	top=parse_cast_expression();
	while((peek()=='*'||peek()=='/'))
	{
		struct astnode *new=newnode(pop(),line_number,2);
		new->child[1]=parse_cast_expression();
		new->child[0]=top;
		top=new;
	}
	return top;
}

struct astnode *parse_cast_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing cast expression\n");
	if(peek()=='(')
	{
		pop();
		switch(peek())
		{
			case TOK_INT:
			case TOK_CHAR:
			case TOK_LONG:
			case TOK_SHORT:
			case TOK_STRUCT:
			case TOK_VOID:
			{
				//fprintf(stderr,"\tParsing cast expression a\n");
				struct astnode *typename=parse_type_name();
				if(peek()!=')')
				{
					parser_error("Expected ')' not '%T'",peek(),line_number);
				}
				else pop();
				struct astnode *exp=parse_cast_expression();
				struct astnode *node=newnode(SYM_CAST,line_number,2);
				node->child[0]=exp;
				node->child[1]=typename;
				return node;
			}
			default:
			{
				//fprintf(stderr,"\tParsing cast expression b\n");
				push('(');
				return parse_unary_expression();
			}
		}
	}
	else 
	{
		//fprintf(stderr,"\tParsing cast expression c\n");
		return parse_unary_expression();
	}
}

struct astnode *parse_unary_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing unary expression\n");
	int pk=peek();
	if((pk=='-')||(pk=='~')||(pk=='!')||(pk=='&')||(pk=='*'))
	{
		pop();
		int id;
		switch(pk)
		{
			case '-': id=SYM_NEGATE; 	break;
			case '&': id=SYM_ADDRESS; 	break;
			case '*': id=SYM_INDIRECT;	break;
			default : id=pk; 			break;
		}
		struct astnode *node=newnode(id,line_number,1);
		node->child[0]=parse_unary_expression();
		return node;
	}
	else if(pk==TOK_SIZEOF)
	{
		pop();
		if(peek()!='(')
		{
			error("Only 'sizeof ( <abstract-type> )' is currently supported",line_number);
			return NULL;
		}
		pop();
		struct astnode *decl=parse_type_name();
		if(peek()!=')')
		{
			parser_error("Expected ')' not %T for sizeof",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *node=newnode(SYM_SIZEOF,line_number,2);
		node->child[0]=decl;
		return node;
	}
	else 
		return parse_postfix_expression();
	
}

struct astnode *parse_postfix_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing postfix expression\n");
	struct astnode *top=parse_primary_expression();
	struct astnode *new;
	for(int tk=peek();;tk=peek())
	{
		if(tk=='(')
		{
			pop();
			struct astlist *parameters=parse_expression_list();
			if(peek()!=')')
				parser_error("Expected ')' not %T",pop(),line_number);
			pop();
			new=newnode(SYM_FUNCTION_CALL,line_number,2);
			new->child[0]=top;
			new->child[1]=parameters;
			top=new;
		}
		else if(tk=='[')
		{
			pop();
			struct astnode *exp=parse_expression();
			if(peek()!=']')
				parser_error("Expected ']' not %T",pop(),line_number);
			pop();
			new=newnode(SYM_ARRAY,line_number,2);
			new->child[0]=top;
			new->child[1]=exp;
			top=new;
		}
		else if(tk=='.')
		{
			pop();
			if(peek()!=TOK_ID)
				parser_error("Expected identifier not %T",peek(),line_number);
			pop();
			char *str=strdup(lex_buffer);
			new=newnode(SYM_DOT,line_number,2);
			new->child[0]=top;
			new->child[1]=str;
			top=new;
		}
		else if(tk==TOK_ARROW)
		{
			pop();
			if(peek()!=TOK_ID)
				parser_error("Expected identifier not %T",peek(),line_number);
			pop();
			char *str=strdup(lex_buffer);
			new=newnode(SYM_ARROW,line_number,2);
			new->child[0]=top;
			new->child[1]=str;
			top=new;
		}
		else
			break;
	}
	return top;
}

struct astlist *parse_expression_list()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing expression list \n");
	if(peek()==')')
		return NULL;
	struct astnode *exp=parse_assignment_expression();
	if(peek()==',')
		pop();
	else if(peek()!=')')
		parser_error("Unexpected symbol '%T' in expression list",peek(),line_number);
	return newastlist(SYM_EXPRESSION_LIST,line_number,exp,parse_expression_list());
}

struct astnode *parse_primary_expression()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"\tParsing primary expression\n");
	if(peek()==TOK_NUMBER)
	{
		pop();
		return (void *)newconst(SYM_NUM,line_number,lex_number);
	}
	else if(peek()==TOK_ID)
	{
		pop();
		struct astnode *node=newnode(SYM_ID,line_number,1);
		node->child[0]=strdup(lex_buffer);
		return node;
	}
	else if(peek()=='(')
	{
		pop();
		struct astnode *node=parse_expression();
		if(peek()!=')')
		{
			parser_error("Expected ')' not %T after '('",pop(),line_number);
			return NULL;
		}
		pop();
		return node;
	}
	else if(peek()==TOK_STRING)
	{
		pop();
		struct astnode *node=newnode(SYM_STRING,line_number,1);
		node->child[0]=strdup(lex_buffer);
		return node;
	}
	else
	{
		parser_error("Expected primary expression not %T",pop(),line_number);
		return NULL;
	}
}