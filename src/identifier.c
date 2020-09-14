#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type.h"
#include "node.h"
#include "identifier.h"
#include "eval.h"
#include "struct.h"

int strcmp(char *s1,char *s2);

struct identifier_list
{
	char *name;
	struct typelist *type;
	struct identifier_list *next;
	int size;
	int loc;
	char defined;
};

struct identifier_block
{
	struct identifier_list *list;
	struct identifier_block *next;
};

struct identifier_block *blocks;
struct identifier_list *globals;
struct identifier_list *function_args;
int id_stack=0;
int func_arg_count=0;

void enter_block()
{
	struct identifier_block *new=calloc(1,sizeof(struct identifier_block));
	if(new==NULL)
	{
		fprintf(stderr,"Fatal Error: out of memory\n");
		exit(-1);
	}
	new->next=blocks;
	blocks=new;
	fprintf(stderr,"enter_block\n");
	return;
}

int leave_block()
{
	int size=0;
	struct identifier_block *next_block=blocks->next;
	struct identifier_list *next;
	for(struct identifier_list *list=blocks->list;list!=NULL;list=next)
	{
		next=list->next;
		size=size+list->size;
		free(list);
	}
	id_stack=id_stack-size;
	free(blocks);
	blocks=next_block;
	fprintf(stderr,"leave block. Size=%d\n",size);
	return size;
}

int local_array_sizeof(struct typelist *type)
{
	switch(type->type)
	{
		case TYPE_CHAR		:	return 1;	 	
		case TYPE_SHORT		:	return gen_size->short_size;
		case TYPE_INT		:	return gen_size->int_size;
		case TYPE_LONG		:	return gen_size->long_size;
		case TYPE_POINTER	:	return gen_size->pointer_size;
		case TYPE_STRUCT	:	return find_struct_size(type->structname);
		default:
			fprintf(stderr,"Unkown type\n");
			return 0;
	}
}

int local_array_size(struct typelist *type,int first)
{
	if(type==NULL)
		return gen_size->int_size;
	if(type->type!=TYPE_ARRAY)
		return local_array_sizeof(type);
	int size=((struct constnode *)type->val)->val;
	size=size*local_array_size(type->next,0);
	if(first)
	{
		int offset;
		for(offset=size;offset>gen_size->int_size;offset=offset-gen_size->int_size);
		size=size+(gen_size->int_size-offset);
	}
	return size;
}

int local_sizeof(struct typelist *type)
{
	if((type->type==TYPE_INT)||(type->type==TYPE_CHAR)||(type->type==TYPE_SHORT))
		return gen_size->int_size;
	else if(type->type==TYPE_LONG)
		return gen_size->long_size;
	else if(type->type==TYPE_POINTER)
		return gen_size->pointer_size;
	else if(type->type==TYPE_ARRAY)
	{
		fprintf(stderr,"local sizeof:");
		return local_array_size(type,1);
	}
	else if(type->type==TYPE_STRUCT)
	{
		int size=find_struct_size(type->structname);
		return size+gen_size->int_size;
	}
	else if(type->type==TYPE_FUNCTION)
		return 0;
	else 
		return gen_size->int_size;
}

int local_stack_loc(int stack,struct typelist *type,int increment)
{
	int size;
	if(gen_size->stack_downward)
	{
		if(type->type==TYPE_ARRAY)
		{
			for(;type->type==TYPE_ARRAY;type=type->next);
			size=local_sizeof(type);
			return stack+increment-size;
		}
		else if(type->type==TYPE_STRUCT)
		{
			//size=find_first_member_size(type->structname);
			return stack+increment-gen_size->int_size;
		}
	}
	return stack;
}

void add_local_id(char *name,struct typelist *type)
{
	struct identifier_list *new=calloc(1,sizeof(struct identifier_list));
	if(new==NULL)
	{
		fprintf(stderr,"Fatal Error: out of memory\n");
		exit(-1);
	}
	new->name=name;
	new->type=type;
	new->next=blocks->list;
	new->size=local_sizeof(type);
	new->loc=local_stack_loc(id_stack,type,new->size);
	id_stack=id_stack+new->size;
	blocks->list=new;
	fprintf(stderr,"add entry %s with size %d at %d\n",name,new->size,new->loc);
	return;
}

int find_local_id(char *name,struct typelist **type)
{
	for(struct identifier_block *block=blocks;block!=NULL;block=block->next)
		for(struct identifier_list *list=block->list;list!=NULL;list=list->next)
		{
			if(strcmp(name,list->name)==0)
			{
				*type=list->type;
				return list->loc;
			}
		}
	return -1;
}


int id_inblock(char *name)
{
	for(struct identifier_list *list=blocks->list;list!=NULL;list=list->next)
	{
		if(strcmp(name,list->name)==0)
		{
			return 1;
		}
	}
	return 0;
}

void add_global_id(char *name,struct typelist *type,char defined)
{
	struct identifier_list *new=calloc(1,sizeof(struct identifier_list));
	if(new==NULL)
	{
		fprintf(stderr,"Fatal Error: out of memory\n");
		exit(-1);
	}
	new->name=name;
	new->type=type;
	new->next=globals;
	new->size=local_sizeof(type);
	new->defined=defined;
	globals=new;
	fprintf(stderr,"add global entry %s\n",name);
	print_type(type);
	return;
}

int is_global(char *name,struct typelist **type)
{
	for(struct identifier_list *list=globals;list!=NULL;list=list->next)
	{
		if(strcmp(name,list->name)==0)
		{
			*type=list->type;
			return 1;
		}
	}
	return 0;
}


void define_global(char *name,int define)
{
	for(struct identifier_list *list=globals;list!=NULL;list=list->next)
	{
		if(strcmp(name,list->name)==0)
		{
			list->defined=define;
			return;
		}
	}
	return;
}

int is_defined(char *name)
{
	for(struct identifier_list *list=globals;list!=NULL;list=list->next)
	{
		if(strcmp(name,list->name)==0)
		{
			return list->defined;
		}
	}
	return 0;
}

void remove_globals()
{
	struct identifier_list *next;
	for(struct identifier_list *list=globals;list!=NULL;list=next)
	{
		next=list->next;
		free(list);
	}
	globals=NULL;
	return;
}

void add_argument_id(char *name,struct typelist *type)
{
	
	struct identifier_list *new=calloc(1,sizeof(struct identifier_list));
	if(new==NULL)
	{
		fprintf(stderr,"Fatal Error: out of memory\n");
		exit(-1);
	}
	new->name=name;
	new->type=type;
	new->next=function_args;
	new->size=local_sizeof(type);
	new->loc=func_arg_count;
	func_arg_count=func_arg_count+1;
	function_args=new;
	fprintf(stderr,"add function argument entry %s at %d\n",name,new->loc);
	return;
}

int find_argument_id(char *name,struct typelist **type)
{
	
	for(struct identifier_list *list=function_args;list!=NULL;list=list->next)
	{
		if(strcmp(name,list->name)==0)
		{
			*type=list->type;
			return list->loc;
		}
	}
	return -1;
}

void remove_arguments()
{
	struct identifier_list *next;
	for(struct identifier_list *list=function_args;list!=NULL;list=next)
	{
		next=list->next;
		free(list);
	}
	function_args=NULL;
	func_arg_count=0;
	return;
}