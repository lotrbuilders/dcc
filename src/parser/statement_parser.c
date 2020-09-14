#include <stddef.h>
#include <stdio.h>
#include "tokens.h"
#include "lexer.h"
#include "errorhandling.h"
#include "type.h"
#include "node.h"
#include "symbols.h"

struct astnode *parse_expression();
struct astnode *parse_declaration();
int peek();
int pop();

struct astlist *parse_block_item_list();
struct astnode *parse_block_item();
struct astnode *parse_local_declaration();
struct astnode *parse_statement();
struct astnode *parse_selection_statement();
struct astnode *parse_labeled_statement();
struct astnode *parse_jump_statement();
struct astnode *parse_iteration_statement();
struct astnode *parse_expression_statement();

int PARSER_DEBUG;


struct astnode *parse_compound_statement()
{
	if(peek()!='{')
	{
		parser_error("Expected '{' not %T",pop(),line_number);
		return NULL;
	}
	pop();
	struct astnode *node=newnode(SYM_COMPOUND_STATEMENT,line_number,3);
	node->child[0]=parse_block_item_list();
	if(peek()!='}')
	{
		parser_error("Expected '}' not %T",pop(),line_number);
		return NULL;
	}
	pop();
	return node;
}

struct astlist *parse_block_item_list()
{
	if(peek()=='}')
		return NULL;
	else 
	{
		struct astnode *statement=parse_block_item();
		return newastlist(SYM_BLOCK_ITEM_LIST,line_number,statement,parse_block_item_list());
	}
}

struct astnode *parse_block_item()
{
	switch(peek())
	{
		case TOK_INT:
		case TOK_CHAR:
		case TOK_LONG:
		case TOK_SHORT:
		case TOK_STRUCT:
		case TOK_VOID:
			return parse_local_declaration();
		default:
			return parse_statement();
	}
}

struct astnode *parse_local_declaration()
{
	struct astnode *declaration=parse_declaration();
	if(peek()!=';')
	{
		parser_error("Expected ';' not %T",pop(),line_number);
		return NULL;
	}
	else 
	{
		pop();
		struct astnode *node=newnode(SYM_LOCAL_DECLARATION,line_number,3);
		//node->child[0]//name
		node->child[1]=declaration;
		//node->child[2]//initialization
		return node;
	}
	
}

struct astnode *parse_statement()
{
	switch(peek())
	{
		case TOK_RETURN:
		case TOK_BREAK:
		case TOK_CONTINUE:
			return parse_jump_statement();
		case TOK_IF:
		case TOK_SWITCH:
			return parse_selection_statement();
		case TOK_CASE:
		case TOK_DEFAULT:
			return parse_labeled_statement();
		case TOK_DO:
		case TOK_WHILE:
		case TOK_FOR:
			return parse_iteration_statement();
		case '{':
			return parse_compound_statement();
		default:
			return parse_expression_statement();
	}
}

struct astnode *parse_jump_statement()
{
	if(peek()==TOK_RETURN)
	{
		pop();
		struct astnode *node=newnode(SYM_RETURN,line_number,1);
		if(peek()==';')
		{
			pop();
			node->child[0]=NULL;
			return node;
		}
		node->child[0]=parse_expression();
		if(peek()!=';')
		{
			parser_error("Expected ';' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		return node;
	}
	else if(peek()==TOK_BREAK)
	{
		pop();
		if(peek()!=';')
		{
			parser_error("Expected ';' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		return newnode(SYM_BREAK,line_number,0);
	}
	else if(peek()==TOK_CONTINUE)
	{
		pop();
		if(peek()!=';')
		{
			parser_error("Expected ';' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		return newnode(SYM_CONTINUE,line_number,0);
	}
	else 
	{
		error("parse jump statement called without valid token",line_number);
		return NULL;
	}

}

struct astnode *parse_selection_statement()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse selection statement\n");
	if(peek()==TOK_IF)
	{
		if(PARSER_DEBUG)
			fprintf(stderr,"parse if statement\n");
		int line=line_number;
		pop();
		if(peek()!='(')
		{
			parser_error("Expected '(' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *exp=parse_expression();
		if(peek()!=')')
		{
			parser_error("Expected ')' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *statement=parse_statement();
		if(peek()==TOK_ELSE)
		{
			if(PARSER_DEBUG)
				fprintf(stderr,"parse else statement\n");
			pop();
			struct astnode *else_stat=parse_statement();
			struct astnode *node=newnode(SYM_IF_ELSE,line,3);
			node->child[0]=exp;
			node->child[1]=statement;
			node->child[2]=else_stat;
			return node;
		}
		else 
		{
			struct astnode *node=newnode(SYM_IF,line,2);
			node->child[0]=exp;
			node->child[1]=statement;
			return node;
		}
	}
	else if(peek()==TOK_SWITCH)
	{
		if(PARSER_DEBUG)
			fprintf(stderr,"parse switch statement\n");
		pop();
		if(peek()!='(')
		{
			parser_error("Expected '(' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *exp=parse_expression();
		if(peek()!=')')
		{
			parser_error("Expected ')' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		int line=line_number;
		struct astnode *statement=parse_statement();
		struct astnode *node=newnode(SYM_SWITCH,line,3);
		node->child[0]=exp;
		node->child[1]=statement;
		node->child[2]=NULL;//Will contain switch target description after semantic analysis
		return node;
	}
	else
	{
		error("parse selection statement called without valid token",line_number);
		return NULL;
	}
}

struct astnode *parse_labeled_statement()
{
	if(PARSER_DEBUG)
		fprintf(stderr,"parse labeled statement\n");
	if(peek()==TOK_CASE)
	{
		pop();
		struct astnode *exp=parse_expression();
		int line=line_number;
		if(peek()!=':')
		{
			parser_error("Expected ')' not %T",peek(),line_number);
		}
		else pop();
		struct astnode *statement=parse_statement();
		struct astnode *node=newnode(SYM_CASE,line,3);
		node->child[0]=exp;
		node->child[1]=statement;
		node->child[2]=NULL;//Will contain target label after semantic analysis
		return node;
	}
	else if(peek()==TOK_DEFAULT)
	{
		pop();
		int line=line_number;
		if(peek()!=':')
		{
			parser_error("Expected ')' not %T",peek(),line_number);
		}
		else pop();
		struct astnode *statement=parse_statement();
		struct astnode *node=newnode(SYM_DEFAULT,line,3);
		//node->child[0]=exp;//Same for later consistency
		node->child[1]=statement;
		node->child[2]=NULL;//Will contain target label after semantic analysis
		return node;
	}
	else 
	{
		error("parse labeled statement called without valid token",line_number);
		return NULL;
	}
}

struct astnode *parse_iteration_statement()
{
	if(peek()==TOK_WHILE)
	{
		if(PARSER_DEBUG)
			fprintf(stderr,"parse while statement\n");
		pop();
		int line=line_number;
		if(peek()!='(')
		{
			parser_error("Expected '(' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *exp=parse_expression();
		if(peek()!=')')
		{
			parser_error("Expected ')' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *statement=parse_statement();
		struct astnode *node=newnode(SYM_WHILE,line,2);
		node->child[0]=exp;
		node->child[1]=statement;
		return node;
	}
	else if(peek()==TOK_DO)
	{
		if(PARSER_DEBUG)
			fprintf(stderr,"parse do-while statement\n");
		pop();
		int line=line_number;
		struct astnode *statement=parse_statement();
		if(peek()!=TOK_WHILE)
		{
			parser_error("Expected 'while' not %T after do",pop(),line_number);
			return NULL;
		}
		pop();
		if(peek()!='(')
		{
			parser_error("Expected '(' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *exp=parse_expression();
		if(peek()!=')')
		{
			parser_error("Expected ')' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		if(peek()!=';')
		{
			parser_error("Expected ';' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		struct astnode *node=newnode(SYM_DO_WHILE,line,2);
		node->child[0]=exp;
		node->child[1]=statement;
		return node;
	}
	else if(peek()==TOK_FOR)
	{
		if(PARSER_DEBUG)
			fprintf(stderr,"parse for statement\n");
		pop();
		
		int line=line_number;
		
		if(peek()!='(')
		{
			parser_error("Expected '(' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		
		struct astnode *start;
		switch(peek())
		{
			case TOK_INT:
			case TOK_CHAR:
			case TOK_LONG:
			case TOK_SHORT:
			case TOK_STRUCT:
			case TOK_VOID:
				start=parse_local_declaration();
				break;
			default:
				start=parse_expression_statement();
		}
		struct astnode *condition=parse_expression_statement();
		struct astnode *iteration;
		if(peek()==')')
			iteration=NULL;
		else 
			iteration=parse_expression();
		
		if(peek()!=')')
		{
			parser_error("Expected ')' not %T",pop(),line_number);
			return NULL;
		}
		pop();
		
		struct astnode *statement=parse_statement();
		struct astnode *node=newnode(SYM_FOR,line,4);
		node->child[0]=start;
		node->child[1]=condition;
		node->child[2]=iteration;
		node->child[3]=statement;
		return node;
	}
	else 
	{
		error("parse selection statement called without valid token",line_number);
		return NULL;
	}
}

struct astnode *parse_expression_statement()
{
	if(peek()==';')
	{
		pop();
		return NULL;
	}
	struct astnode *node=parse_expression();
	if(peek()!=';')
	{
		parser_error("Expected ';' not %T",pop(),line_number);
		return NULL;
	}
	pop();
	return node;
}