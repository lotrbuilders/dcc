/*
	Node generation DCC
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
#include "type.h"
#include "node.h"

struct astnode *newnode(int id,int line_number,int child_count)
{
	struct astnode *ptr=calloc(1,sizeof(struct astnode)+(child_count-1)*sizeof(void *));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	ptr->id=id;
	ptr->line=line_number;
	ptr->child_count=child_count;
	return ptr;
}

struct constnode *newconst(int id,int line_number,long val)
{
	struct constnode *ptr=calloc(1,sizeof(struct constnode));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	ptr->id=id;
	ptr->line=line_number;
	ptr->val=val;
	return ptr;
}

struct astlist *newastlist(int id,int line_number,void *child,void *next)
{
	struct astlist *ptr=calloc(1,sizeof(struct astlist));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	ptr->id=id;
	ptr->line=line_number;
	ptr->child=child;
	ptr->next=next;
	return ptr;
}

struct astlist *attach_astlist(struct astlist *a,struct astlist *b)
{
	if(a==NULL)
		return b;
	struct astlist *list;
	for(list=a;list->next!=NULL;list=list->next);
	list->next=b;
	return a;
}
#include "eval.h"
struct astlist *make_astlist(struct astnode *node)
{
	if(node->id==GEN_LIST)
	{
		return (void *)node;
	}
	else
	{
		return newastlist(GEN_LIST,node->line,node,NULL);
	}
}
