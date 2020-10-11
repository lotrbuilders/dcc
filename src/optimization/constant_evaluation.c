/*
	Constant evaluation DCC
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
#include <stdlib.h>
#include <stdio.h>
#include "errorhandling.h"
#include "symbols.h"
#include "type.h"
#include "node.h"
#include "identifier.h"

int eval_type(struct typelist *list);

int DEBUG_CONSTANT_EVAL=0;


int is_constant(struct astnode *ast)
{
	if(ast==NULL)
		return 0;
	if(ast->id==SYM_NUM)
		return 1;
	else 
		return 0;
}

long constant_value(struct constnode *ast)
{
	return ast->val;
}

int local_array_size(struct typelist *type,int first);
void *constant_evaluation(struct astnode *ast)
{
	if(ast==NULL)
		return ast;
	if(DEBUG_CONSTANT_EVAL)
		fprintf(stderr,"Evaluating constant\n");
	switch(ast->id)
	{
		case SYM_FUNCTION_CALL:
		case SYM_ADDRESS:
		case SYM_INDIRECT:
		
		case SYM_ID:
		case SYM_STRING:
		case '=':
			//Not possible or not allowed
			break;
			
		case SYM_NUM:
			return ast;
		case SYM_SIZEOF:
		{
			int line=ast->line;
			int size=local_array_size(ast->child[1],0);
			free_type(ast->child[1]);
			free(ast);
			return newconst(SYM_NUM,line,size);
			break;
		}
		case SYM_CAST:
		{
			ast->child[0]=constant_evaluation(ast->child[0]);
			struct typelist *to=ast->type;
			struct typelist *from=((struct astnode *)ast->child[0])->type;
			if(is_constant(ast->child[0]))
			{
				if(is_arithmatic_type(to))
				{
					long shift=local_array_size(to,0);
					shift=8*shift;	//Assume 8-bit bytes
					long mask=0;
					for(int i=0;i<shift;i=i+1)
					{
						mask=mask*2;
						mask=mask+1;
					}
					long val=((struct constnode *)ast->child[0])->val;
					val=val&mask;
					free_type(from);
					free(ast->child[0]);
					return newconst(SYM_NUM,ast->line,val);
				}
				else
				{
					free_type(from);
					((struct astnode *)ast->child[0])->type=to;
					return ast->child[0];
				}
			}
			break;
		}
		
		case '~':
		case '!':
		case SYM_NEGATE:
			ast->child[0]=constant_evaluation(ast->child[0]);
			if(is_constant(ast->child[0]))
			{
				int id=ast->id;
				struct constnode *node=ast->child[0];
				free_type(node->type);
				node->type=ast->type;
				free(ast);
				switch(id)
				{
					case '~': 			node->val=~node->val;	break;
					case '!': 			node->val=!node->val;	break;
					case SYM_NEGATE: 	node->val=-node->val;	break;
				}
				return node;
			}
			break;
		case '+':
		case '-':
		
		case '&':
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
			ast->child[0]=constant_evaluation(ast->child[0]);
			ast->child[1]=constant_evaluation(ast->child[1]);
			if(is_constant(ast->child[0])&&is_constant(ast->child[1]))
			{
				int id=ast->id;
				struct constnode *left=ast->child[0];
				struct constnode *right=ast->child[1];
				struct constnode *node=newconst(SYM_NUM,ast->line,0);
				node->type=ast->type;
				free(ast);
				free_type(left->type);
				free_type(right->type);
				if(DEBUG_CONSTANT_EVAL)
					fprintf(stderr,"Evaluating constant still\n");
				switch(id)
				{
					case '+':		node->val=left->val+right->val;		break;
					case '-':		node->val=left->val-right->val;		break;
					case '*':		node->val=left->val*right->val;		break;
					case '/':		node->val=left->val/right->val;		break;
					case '&':		node->val=left->val&right->val;		break;
					case SYM_LAND:	node->val=left->val&&right->val;	break;
					case SYM_LOR:	node->val=left->val||right->val;	break;
					case SYM_EQ:	node->val=left->val==right->val;	break;
					case SYM_NE:	node->val=left->val!=right->val;	break;
					case '>':		node->val=left->val>right->val;		break;
					case '<':		node->val=left->val<right->val;		break;
					case SYM_LE:	node->val=left->val<=right->val;	break;
					case SYM_GE:	node->val=left->val>=right->val;	break;
				}
				free(left);
				free(right);
				return node;
			}
			break;
		case SYM_SEL:
			//TODO free one side if condition is constant
			break;
		
		default:
			error("Unkown symbol whilst collapsing constants",ast->line);
	}
	return ast;
}