/*
	Struct storage and checking DCC
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
#include "symbols.h"
#include "errorhandling.h"

int strcmp(char *s1,char *s2);

struct memberlist
{
	char *name;
	struct typelist *type;
	struct memberlist *next;
	int location;
};

struct structlist
{
	char *name;
	struct memberlist *member;
	struct structlist *next;
	int size;
} *global_structlist;

int struct_is_defined(char *name)
{
	for(struct structlist *list=global_structlist;list!=NULL;list=list->next)
	{
		if(!strcmp(list->name,name))
			return 1;
	}
	return 0;
}

struct typelist *check_declaration(struct astnode *ast,char **name,struct astnode **initialization);

struct memberlist *getmembers(struct astlist *definition, int *size)
{
	fprintf(stderr,"Get struct members\n");
	struct memberlist *members=NULL;
	int location=0;
	for(struct astlist *list=definition;list!=NULL;list=list->next)
	{
		struct typelist *type;
		char *name=NULL;
		void *initialization=NULL;
		fprintf(stderr,"Get struct member \n");
		type=check_declaration(list->child,&name,(void *)&initialization);
		if(initialization!=NULL)
			error("Unexpected initialization in struct definition",list->line);
		else if(type->type==TYPE_VOID)
			error("Unexpected void in struct definition",list->line);
		struct memberlist *ptr=calloc(1,sizeof(struct memberlist));
		if(ptr==NULL)
		{
			fprintf(stderr,"Fatal error: out of memory\n");
			exit(-1);
		}
		fprintf(stderr,"Member %s with ",name);
		print_type(type);
		ptr->name=name;
		ptr->type=type;
		fprintf(stderr,"location %d\n",location);
		location=location+align_type(type,location);
		fprintf(stderr,"location %d\n",location);
		ptr->location=location;
		location=location+local_array_size(type,0);
		fprintf(stderr,"location %d\n",location);
		ptr->next=members;
		members=ptr;
	}
	*size=location;
	return members;
}

void define_struct(char *name,void *definition,int line)
{
	for(struct structlist *list=global_structlist;list!=NULL;list=list->next)
	{
		if(!strcmp(list->name,name))
		{
			error("Struct is defined multiple times",line);
			return;
		}
	}
	struct structlist *ptr=calloc(1,sizeof(struct structlist));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	fprintf(stderr,"Struct name %s\n",name);
	ptr->name=name;
	
	ptr->next=global_structlist;
	global_structlist=ptr;
	ptr->member=getmembers(definition,&ptr->size);
	struct typelist *type=new_type(TYPE_INT,NULL,NULL);
	int size=ptr->size;
	ptr->size=size+align_type(type,size);
	return;
}

struct typelist *find_struct_member(char *structname,char *member_name,int *offset)
{
	fprintf(stderr,"Find struct member %s in struct %s\n",member_name,structname);
	for(struct structlist *list=global_structlist;list!=NULL;list=list->next)
	{
		fprintf(stderr,"Check struct\n");
		fprintf(stderr,"%s==%s\n",structname,list->name);
		if(!strcmp(list->name,structname))
		{
			fprintf(stderr,"Check members\n");
			for(struct memberlist *memb=list->member;memb!=NULL;memb=memb->next)
			{
				fprintf(stderr,"Check member\n");
				fprintf(stderr,"%s==%s\n",memb->name,member_name);
				if(!strcmp(memb->name,member_name))
				{
					*offset=memb->location;
					return memb->type;
				}
			}
			return NULL;
		}
	}
	return NULL;
}

int find_struct_size(char *name)
{
	for(struct structlist *list=global_structlist;list!=NULL;list=list->next)
	{
		if(!strcmp(list->name,name))
		{
			return list->size;
		}
	}
	return 0;
}

int local_array_sizeof(struct typelist *type);
//First member is last in list;
int find_first_member_size(char *name)
{
	for(struct structlist *list=global_structlist;list!=NULL;list=list->next)
	{
		if(!strcmp(list->name,name))
		{
			struct memberlist *member=list->member;
			if(member==NULL)
				return 0;
			for(;member->next!=NULL;member=member->next);
			struct typelist *type=member->type;
			if(type->type==TYPE_STRUCT)
				return find_first_member_size(type->structname);
			for(;type->type==TYPE_ARRAY;type=type->next);
			return local_array_sizeof(type);
		}
	}
	return 0;
}