/*
	Switch handling and checking DCC
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
#include "switch.h"
#include "errorhandling.h"

int global_label_count;
struct switchframe_t
{
	struct switchlist *list;
	struct switchframe_t *next;
	int line;
} *switchframe=NULL;

void enter_switch(int line)
{
	struct switchframe_t *ptr=calloc(1,sizeof(struct switchframe_t));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	ptr->list=NULL;
	ptr->next=switchframe;
	ptr->line=line;
	switchframe=ptr;
	in_switch=1;
	return;
}

void print_switch(struct switchlist *input)
{
	fprintf(stderr,"Printing switch\n");
	for(struct switchlist *list=input;list!=NULL;list=list->next)
	{
		fprintf(stderr,"%p\n",list);
		if(list->def)
			fprintf(stderr,"default: ");
		else 
		{
			fprintf(stderr,"case %d: ",list->val);
		}
		fprintf(stderr,"@ L%d\n",list->label);
	}
	return;
}

struct switchlist *leave_switch()
{
	//print_switch(switchframe->list);
	struct switchlist *list=switchframe->list;
	void *tmp=switchframe;
	switchframe=switchframe->next;
	free(tmp);
	if(switchframe==NULL)
		in_switch=0;
	//print_switch(list);
	return list;
}

int add_case(int val)
{
	//print_switch(switchframe->list);
	//fprintf(stderr,"add case: %d @ L%d\n",val,global_label_count);
	struct switchlist *ptr=calloc(1,sizeof(struct switchlist));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	ptr->def=0;
	ptr->val=val;
	ptr->label=global_label_count;
	global_label_count=global_label_count+1;
	
	if(switchframe->list==NULL)
	{
		switchframe->list=ptr;
		return ptr->label;
	}
	
	struct switchlist *list;
	for(list=switchframe->list;(list->next!=NULL)&&(list->next->def==0)&&(list->next->val<val);list=list->next);
	if(list->next==NULL)
		list->next=ptr;
	else if(list->next->val==val)
	{
		error("Switch value defined twice",switchframe->line);
		free(ptr);
	}
	else
	{
		ptr->next=list->next;
		list->next=ptr;
	}
	
	return ptr->label;
}

int add_default()
{
	//print_switch(switchframe->list);
	//fprintf(stderr,"add default: @ L%d\n",global_label_count);
	struct switchlist *ptr=calloc(1,sizeof(struct switchlist));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	ptr->def=1;
	ptr->val=0;
	ptr->label=global_label_count;
	global_label_count=global_label_count+1;
	
	if(switchframe->list==NULL)
	{
		fprintf(stderr,"empty case list\n");
		switchframe->list=ptr;
		return ptr->label;
	}
	
	struct switchlist *list;
	for(list=switchframe->list;(list->next!=NULL)&&(list->next->def==0);list=list->next);
	if(list->next==NULL)
		list->next=ptr;
	else if(list->next->def)
	{
		error("Switch default defined twice",switchframe->line);
		free(ptr);
	}
	else 
	{
		fprintf(stderr,"weird case list\n");
	}
	//print_switch(switchframe->list);
	//print_switch(ptr);
	return ptr->label;
}