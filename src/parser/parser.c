/*
	Top level parser DCC
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

struct astlist *parse_translation_unit();
struct astnode *parse_global();

struct astnode *parse_declaration();

struct astnode *parse_compound_statement();

int saved_token;
int pushed_token;

int PARSER_DEBUG=0;
int PEEK_DEBUG=0;

int peek()
{
	if(pushed_token)
	{
		if(PEEK_DEBUG)
			fprintf(stderr,"\t\tpeek: pushed %s\n",token_to_string(saved_token));
		return pushed_token;
	}
	else if(saved_token==0)
	{
		saved_token=lex();
		if(PEEK_DEBUG)
			fprintf(stderr,"\t\tpeek: %s\n",token_to_string(saved_token));
		return saved_token;
	}
	else 
	{
		if(PEEK_DEBUG)
			fprintf(stderr,"\t\tpeek: %s\n",token_to_string(saved_token));
		return saved_token;
	}
}

int pop()
{
	if(pushed_token)
	{
		if(PEEK_DEBUG)
			fprintf(stderr,"\t\tpeek: pushed %s\n",token_to_string(saved_token));
		int tmp=pushed_token;
		pushed_token=0;
		return tmp;
	}
	if(saved_token==0)
	{
		int tmp=lex();
		if(PEEK_DEBUG)
			fprintf(stderr,"\t\tpop: %s\n",token_to_string(tmp));
		return tmp;
	}
	else 
	{
		int tk=saved_token;
		saved_token=0;
		if(PEEK_DEBUG)
			fprintf(stderr,"\t\tpop: %s\n",token_to_string(tk));
		return tk;
	}
}

void push(int tk)
{
	if(PEEK_DEBUG)
		fprintf(stderr,"\t\tpush: %s\n",token_to_string(tk));
	if(pushed_token)
		error("Token stack full",line_number);
	else pushed_token=tk;
	return;
}

struct astlist *parse()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parsing\n");
	return parse_translation_unit();
}

struct astlist *parse_translation_unit()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse translation_unit\n");
	if(peek()==TOK_EOF)
		return NULL;
	else
	{
		struct astnode *global=parse_global();
		
		return newastlist(SYM_TRANSLATION_UNIT, line_number,
						global, parse_translation_unit());
	}
}

struct astnode *parse_global()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse global\n");
	int line=line_number;
	struct astnode *declaration=parse_declaration();
	if(declaration->id==SYM_TYPE_DECLARATION)
	{
		if(peek()!=';')
		{
			parser_error("Expected ';' not '%T'",peek(),line_number);
		}
		pop();
		return declaration;
	}
	if(peek()=='{')
	{
		struct astnode *node=newnode(SYM_FUNCTION_DEFENITION,line,3);
		//node->child[0];//name;
		node->child[1]=declaration;
		node->child[2]=parse_compound_statement();
		return node; 
	}
	else 
	{
		if(peek()==';')
		{
			pop();
			struct astnode *node=newnode(SYM_GLOBAL_DECLARATION,line,3);
			//node->child[0];//name;
			node->child[1]=declaration;
			//node->child[2];//initialization
			return node;
		}
		else 
		{
			parser_error("Expected ';' or '{' not %T",pop(),line_number);
			return NULL;
		}
	}
}














