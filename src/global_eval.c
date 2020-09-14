#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "node.h"
#include "identifier.h"
#include "eval.h"

int strcmp(char *s1,char *s2);
int eval_type(struct typelist *type);

struct identifier_list
{
	char *name;
	struct typelist *type;
	struct identifier_list *next;
	long val;
	char defined;
};

struct identifier_list *global_eval_list;

void global_eval_add(char *name,struct typelist *type,char defined,long val)
{
	struct identifier_list *new=calloc(1,sizeof(struct identifier_list));
	if(new==NULL)
	{
		fprintf(stderr,"Fatal Error: out of memory\n");
		exit(-1);
	}
	new->name=name;
	new->type=type;
	new->val=val;
	new->defined=defined;
	new->next=global_eval_list;
	global_eval_list=new;
	fprintf(stderr,"add global eval entry %s\n",name);
	return;
}

void global_eval(char *name,struct typelist *type,char defined,long val)
{
	for(struct identifier_list *list=global_eval_list;list!=NULL;list=list->next)
		if(strcmp(list->name,name)==0)
		{
			if(!list->defined)
			{
				list->defined=defined;
				list->val=val;
			}
			return;
		}
	global_eval_add(name,type,defined,val);
	return;
}

int local_array_size(struct typelist *type,int first);

struct astlist *glob_eval_gen(struct identifier_list *list)
{
	if(list==NULL)
		return NULL;
	if(list->defined&&(list->type->type!=TYPE_FUNCTION))
	{
		struct astnode *node=newnode(GEN_GLOBAL+eval_type(list->type),0,2);
		node->child[0]=list->name;
		node->child[1]=newconst(GEN_NUM,0,list->val);
		return newastlist(GEN_LIST,0,node,glob_eval_gen(list->next));
	}
	else if(list->type->type==TYPE_FUNCTION&&!list->defined)
	{
		struct astnode *node=newnode(GEN_EXTERN,0,1);
		node->child[0]=list->name;
		return newastlist(GEN_LIST,0,node,glob_eval_gen(list->next));
	}
	else if(!list->defined)
	{
		struct astnode *node=newnode(GEN_COMMON,0,2);
		node->child[0]=list->name;
		node->child[1]=newconst(GEN_NUM,0,local_array_size(list->type,0));
		fprintf(stderr,"common size %d\n",local_array_size(list->type,0));
		return newastlist(GEN_LIST,0,node,glob_eval_gen(list->next));
	}
	return glob_eval_gen(list->next);
}

struct astlist *global_eval_gen()
{
	fprintf(stderr,"evaluating globals\n");
	return glob_eval_gen(global_eval_list);
}

