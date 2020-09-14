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
