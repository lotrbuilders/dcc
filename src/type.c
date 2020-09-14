#include <stdlib.h>
#include <stdio.h>
#include "type.h"
#include "node.h"
#include "struct.h"
#include "symbols.h"
#include "errorhandling.h"
#include "eval.h"

struct typelist *new_type(short type,void *val,struct typelist* next)
{
	struct typelist *ptr=calloc(1,sizeof(struct typelist));
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal error: out of memory\n");
		exit(-1);
	}
	ptr->type=type;
	ptr->val=val;
	ptr->next=next;
	return ptr;
}

struct typelist *append_type(struct typelist *list,short type,void *val,struct typelist *next)
{
	struct typelist *ptr=new_type(type,val,next);
	return append_typelist(list,ptr);
}

struct typelist *append_typelist(struct typelist *list1,struct typelist *list2)
{
	if(list1==NULL)
		return list2;
	struct typelist *ptr;
	for(ptr=list1;ptr->next!=NULL;ptr=ptr->next);
	ptr->next=list2;
	return list1;
}

struct typelist *replace_typelist(struct typelist *list1,struct typelist *list2,short type)
{
	struct typelist *ptr;
	if(list1->type==type)
		return list2;
	for(ptr=list1;ptr->next!=NULL;ptr=ptr->next)
	{
		if(ptr->next->type==type)
		{
			free(ptr->next);
			ptr->next=list2;
			return list1;
		}
	}
	ptr->next=list2;
	return list1;
}

struct typelist *insert_typelist(struct typelist *type,struct typelist *insert)
{
	print_type(type);
	if(type==NULL)
		return insert;
	if(type->type==TYPE_ID)
	{
		insert->next=type;
		return insert;
	}
	struct typelist *list;
	for(list=type;(list->next!=NULL)&&(list->next->type!=TYPE_ID);list=list->next);
	struct typelist *temp=list->next;
	list->next=insert;
	insert->next=temp;
	print_type(type);
	return type;
}

char *find_type_identifier(struct typelist *list,int line)
{
	for(struct typelist *ptr=list;ptr!=NULL;ptr=ptr->next)
	{
		if(ptr->type==TYPE_ID)
			return ptr->val;
	}
	error("No identifier in type",line);
	return "_";
}

int compare_type(struct typelist *list1,struct typelist *list2)
{
	fprintf(stderr,"Compare types\n");
	while((list1!=NULL)&&(list2!=NULL))
	{
		if(list1->type!=list2->type)

			return 1;
		list1=list1->next;
		list2=list2->next;
	}
	if((list1==NULL)&&(list2!=NULL))
		return 1;
	else if((list1!=NULL)&&(list2==NULL))
		return 1;
	else 
		return 0;
}

int is_compatible_type(struct typelist *list1,struct typelist *list2)
{
	fprintf(stderr,"Check comparable types\n");
	print_type(list1);
	print_type(list2);
	if(list1->type==TYPE_POINTER&&list2->type==TYPE_POINTER)
		return 0;//pointer types automatically compatible for now;
	else if(list1->type==TYPE_ARRAY&&list2->type==TYPE_POINTER)
		return 0;//pointer and array automatically compatible for now;
	else if(list1->type==TYPE_POINTER&&list2->type==TYPE_ARRAY)
		return 0;//pointer and array automatically compatible for now;
	else if(list1->type==TYPE_ARRAY&&list2->type==TYPE_ARRAY)
		return 0;//array and array automatically compatible for now;
	else if(((list1->type==TYPE_POINTER)&&(list2->type==TYPE_INT))
			||((list2->type==TYPE_POINTER)&&(list1->type==TYPE_INT)))
		return 0;//Pointer and integer are compatible for now. Should generate warnings when they are implemented
	
	while((list1!=NULL)&&(list2!=NULL))
	{
		if(list1->type!=list2->type)
		{
			return !(is_arithmatic_type(list1)&&is_arithmatic_type(list2));
		}
		list1=list1->next;
		list2=list2->next;
	}
	if((list1==NULL)&&(list2!=NULL))
		return 1;
	else if((list1!=NULL)&&(list2==NULL))
		return 1;
	else 
		return 0;
	
}


struct astnode *check_constant_expression(struct astnode *ast);
struct astnode *check_expression(struct astnode *ast);
void *constant_evaluation(struct astnode *ast);

void check_declaration_type(struct typelist *list,int line)
{
	fprintf(stderr,"Check declaration type\n");
	if(list==NULL)
		return;
	if(list->type==TYPE_FUNCTION)
		if(!is_returnable_type(list->next))
			error("Function is not of returnable type",line);
	if(list->type==TYPE_ARRAY)
	{
		check_expression(list->val);
		check_constant_expression(list->val);
		list->val=constant_evaluation(list->val);
		if(((struct constnode *)list->val)->id==SYM_NUM)
			if(((struct constnode *)list->val)->val<=0)
				error("Array with size of zero or less",line);
	}
	if(list->type==TYPE_STRUCT)
	{
		if(list->val!=NULL)//Type not defined
		{
			define_struct(list->structname,list->val,line);
		}
		else if(!struct_is_defined(list->structname))
			error("Undefined struct",line);
	}
	check_declaration_type(list->next,line);
	return;
}


int is_arithmatic_type(struct typelist *list)
{
	if(list==NULL)
		error("list=NULL",0);
	if(list->type==TYPE_FUNCTION)
		return 0;
	else if(list->type==TYPE_POINTER)
		return 0;
	else if(list->type==TYPE_ARRAY)
		return 0;
	else if(list->type==TYPE_STRUCT)
		return 0;
	else if(list->type==TYPE_VOID)
		return 0;
	else return 1;
}

int is_returnable_type(struct typelist *list)
{
	if(list->type==TYPE_FUNCTION)
		return 0;
	else if(list->type==TYPE_ARRAY)
		return 0;
	else if(list->type==TYPE_STRUCT)
		return 0;
	else return 1;
}

void print_type(struct typelist *input)
{
	fprintf(stderr,"Type is ");
	for(struct typelist *list=input;list!=NULL;list=list->next)
	{
		if(list->type==TYPE_INT)
			fprintf(stderr,"int ");
		else if(list->type==TYPE_CHAR)
			fprintf(stderr,"char ");
		else if(list->type==TYPE_SHORT)
			fprintf(stderr,"short ");
		else if(list->type==TYPE_LONG)
			fprintf(stderr,"long ");
		else if(list->type==TYPE_POINTER)
			fprintf(stderr,"* ");
		else if(list->type==TYPE_FUNCTION)
			fprintf(stderr,"() ");
		else if(list->type==TYPE_ARRAY)
			fprintf(stderr,"[] ");
		else if(list->type==TYPE_ID)
			fprintf(stderr,"id:%s ",(char *)list->val);
		else if(list->type==TYPE_STRUCT)
			fprintf(stderr,"struct %s ",(char *)list->structname);
		else if(list->type==TYPE_VOID)
			fprintf(stderr,"void ");
	}
	fprintf(stderr,"\n");
	return;
}


struct typelist *unary_type_promotion(struct typelist *list)
{
	if(list==NULL)
		return NULL;
	if(list->type==TYPE_CHAR)
		return new_type(TYPE_INT,NULL,NULL);
	else if(list->type==TYPE_SHORT)
		return new_type(TYPE_INT,NULL,NULL);
	else if(list->type==TYPE_VOID)
	{
		error("Arithmatic on void function somewhere",-1);
		return new_type(TYPE_INT,NULL,NULL);
	}
	else 
		return list;
}

struct typelist *binary_type_promotion(struct typelist *left,struct typelist *right)
{
	if(!is_arithmatic_type(left))
		return left;
	if(!is_arithmatic_type(right))
		return right;
	if(left->type==TYPE_LONG)
		return left;
	if(right->type==TYPE_LONG)
		return right;
	if(left->type==TYPE_INT)
		return left;
	if(right->type==TYPE_INT)
		return right;
	return left;
}

struct typelist *free_type_list[4096];
int free_type_count;
void free_type(struct typelist *list)
{
	if(free_type_count<4096)
	{
		free_type_list[free_type_count]=list;
		free_type_count=free_type_count+1;
	}
	else 
	{
		fprintf(stderr,"TODO: start keeping only one copy of a type\n");
		exit(-1);
	}
	return;
}

int align_type(struct typelist *type,int location)
{
	if(location==0)
		return 0;
	struct typelist *list;
	for(list=type;list->type==TYPE_ARRAY;list=list->next);
	int alignment;
	switch(list->type)
	{
		case TYPE_INT:		alignment=gen_size->int_align;		break;
		case TYPE_CHAR:		alignment=1;						break;
		case TYPE_POINTER:	alignment=gen_size->pointer_align;	break;
		case TYPE_LONG:		alignment=gen_size->long_align;		break;
		case TYPE_SHORT:	alignment=gen_size->short_align;	break;
		default: 	error("Type without alignment somewher",-1);
					return location;
	}
	int offset=location;
	for(;offset>alignment;offset=offset-alignment);
	return alignment-offset;
}
