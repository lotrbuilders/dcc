/*
	x86 backend DCC
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
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "node.h"
#include "simple_gen.h"
#include "eval.h"

struct gen_struct x86_gen;
struct target_size x86_size;
int x86_stack;
int last_section;
#define SECTION_CODE 1
#define SECTION_DATA 2

void x86_section(int section)
{
	if(section!=last_section)
	{
		if(section==SECTION_CODE)
			printf("section .text\n");
		else if(section==SECTION_DATA)
			printf("section .data\n");
	}
	last_section=section;
	return;
}

void x86_extern(char *name)
{
	printf("\textern %s\n",name);
	return;	
}

void x86_define(char *name,int size,long val)
{
	x86_section(SECTION_DATA);
	printf("\nglobal %s\n",name);
	printf("%s:\n\td%c %d\n",name,size==1?'b':(size==2?'w':'d'),(int)val);
	return;
}

void x86_common(char *name,int size)
{
	x86_section(SECTION_DATA);
	printf("\tcommon %s %d:%d\n",name,size,size>=4?4:size);
	return;
}

void x86_prolog(char *name,struct astnode *arglist)
{
	x86_section(SECTION_CODE);
	printf("\nglobal %s:function (%s.end-%s)\n",name,name,name);
	printf("%s:\n",name);
	puts("\tpush ebp");
	puts("\tmov ebp,esp");
	return;
}

void x86_epilog(char *name)
{
	puts(".end:\n");
	x86_stack=0;
	return;
}

void x86_enter(int size)
{
	if(size!=0)
		printf("\tsub esp,%d\n",size);
	x86_stack=x86_stack+size;
	return;
}

void x86_leave(int size)
{
	if(size!=0)
		printf("\tadd esp,%d\n",size);
	//x86_stack=0;
	return;
}

void x86_return(int size)
{
	puts("\tpop ebp");
	puts("\tret");
	return;
}

void x86_label(int n)
{
	printf(".L%d:\n",n);
	return;
}

void x86_jnz(int n)
{
	puts("\ttest eax,eax");
	printf("\tjnz .L%d\n",n);
	return;
}

void x86_jz(int n)
{
	puts("\ttest eax,eax");
	printf("\tjz .L%d\n",n);
	return;
}

void x86_jump(int n)
{
	printf("\tjmp .L%d\n",n);
	return;
}

void x86_jump_case(int val,int label)
{
	printf("\tcmp eax,%d\n",val);
	printf("\tje .L%d\n",label);
	return;
}

void x86_save_top()
{
	puts("\tpush eax");
	x86_stack=x86_stack+4;
	return;
}

void x86_constant(int n)
{
	printf("\tmov eax,%d\n",n);
	return;
}


void x86_terminal(struct terminal *term)
{
	if(term==0)
		return;
	switch(term->type)
	{
		case GEN_NUM+I+4:
			printf("\tmov eax,%d\n",(int)term->val);
			break;
		case GEN_LOADL+I+4:
		case GEN_LOADL+P+4:
			printf("\tmov eax,[ebp-%d]\n",(int)term->val+4);
			break;
		case GEN_LOADL+I+1:
			printf("\tmovsx eax,byte [ebp-%d]\n",(int)term->val+4);
			break;
		case GEN_LOADL+I+2:
			printf("\tmovsx eax,word [ebp-%d]\n",(int)term->val+4);
			break;
		case GEN_LOADF+I+4:
		case GEN_LOADF+P+4:
			printf("\tmov eax,[ebp+%d]\n",(int)term->val*4+8);
			break;
		case GEN_LOADF+I+1:
			printf("\tmovsx eax, byte[ebp+%d]\n",(int)term->val*4+8);
			break;
		case GEN_LOADF+I+2:
			printf("\tmovsx eax, word[ebp+%d]\n",(int)term->val*4+8);
			break;
		case GEN_LOADG+I+4:
		case GEN_LOADG+P+4:
			printf("\tmov eax,[%s]\n",(char*)term->ptr);
			break;
		case GEN_LOADG+I+1:
			printf("\tmovsx eax,byte [%s]\n",(char*)term->ptr);
			break;
		case GEN_LOADG+I+2:
			printf("\tmovsx eax,word [%s]\n",(char*)term->ptr);
			break;
		case GEN_ADDRL+P+4:
			printf("\tlea eax,[ebp-%d]\n",(int)term->val+4);
			break;
		case GEN_ADDRF+P+4:
			printf("\tlea eax,[ebp+%d]\n",(int)term->val*4+8);
			break;
		case GEN_ADDRG+P+4:
			printf("\tmov eax,%s\n",(char*)term->ptr);
			break;
		case GEN_STRING+P+4:
			printf("\tmov eax,.S%d\n",(int)term->val);
			break;
		
	}
	return;
}

void x86_unary_expression(int op,struct terminal *term)
{
	switch(op)
	{
		case GEN_NEG+I+4:
			x86_terminal(term);
			puts("\tneg eax");
			break;
			
		case GEN_BNOT+I+4:
			x86_terminal(term);
			puts("\tnot eax");
			break;
		
		case GEN_LNOT+I+4:
			x86_terminal(term);
			puts("\tor eax,eax");
			puts("\tsetz al");
			puts("\tmovzx eax,al");
			break;
		case GEN_INDIR+I+4:
		case GEN_INDIR+P+4:
			x86_terminal(term);
			puts("\tmov eax,[eax]");
			break;
		case GEN_INDIR+I+1:
			x86_terminal(term);
			puts("\tmovsx eax,byte [eax]");
			break;
		case GEN_INDIR+I+2:
			x86_terminal(term);
			puts("\tmovsx eax,word [eax]");
			break;
	}
	return;
}

void x86_cast_expression(int to,int from)
{
	if(to<from)
	{
		char *acc=to==1?"al":(to==2?"ax":"eax");
		printf("\tmovsx eax,%s\n",acc);
	}
	return;
}

void x86_mem_imm(struct terminal *term)
{
	if(term==0)
		return;
	switch(term->type)
	{
		case GEN_NUM+I+4:
			printf("%d\n",(int)term->val);
			break;
		case GEN_LOADL+I+4:
		case GEN_LOADL+P+4:
			printf("[ebp-%d]\n",(int)term->val+4);
			break;
		case GEN_LOADF+I+4:
		case GEN_LOADF+P+4:
			printf("[ebp+%d]\n",(int)term->val*4+8);
			break;
		case GEN_LOADG+I+4:
		case GEN_LOADG+P+4:
			printf("[%s]\n",(char*)term->ptr);
			break;
		case GEN_ADDRG+P+4:
			printf("%s\n",(char*)term->ptr);
			break;
		case GEN_STRING+P+4:
			printf(".S%d\n",(int)term->val);
			break;
	}
	return;
}

void x86_expression(int op,struct terminal *term)
{
	switch(op)
	{
		case GEN_ADD+I+4:
		case GEN_ADD+P+4:
			if((term==0)||(term->type==GEN_ADDRL+P+4)||(term->type==GEN_ADDRF+P+4))
			{
				if(term!=0)
				{
					puts("\tpush eax");
					x86_terminal(term);
				}
				else 
					x86_stack=x86_stack-4;
				puts("\tpop ecx");
				puts("\tadd eax,ecx");
			}
			else
			{
				printf("\tadd eax,");
				x86_mem_imm(term);
			}
			break;
			
		case GEN_SUB+I+4:
		case GEN_SUB+P+4:
			if((term==0)||(term->type==GEN_ADDRL+P+4)||(term->type==GEN_ADDRF+P+4))
			{
				if(term!=0)
				{
					puts("\tpush eax");
					x86_terminal(term);
				}
				else 
					x86_stack=x86_stack-4;
				puts("\tpop ecx");
				puts("\tsub eax,ecx");
			}
			else
			{
				printf("\tsub eax,");
				x86_mem_imm(term);
			}
			break;
		
		case GEN_MUL+I+4:
			if((term==0)||(term->type==GEN_ADDRL+P+4)||(term->type==GEN_ADDRF+P+4))
			{
				if(term!=0)
				{
					puts("\tpush eax");
					x86_terminal(term);
				}
				else 
					x86_stack=x86_stack-4;
				puts("\tpop ecx");
				puts("\timul eax,ecx");
			}
			else
			{
				printf("\timul eax,");
				x86_mem_imm(term);
			}
			break;
		case GEN_BAND+I+4:
			if((term==0)||(term->type==GEN_ADDRL+P+4)||(term->type==GEN_ADDRF+P+4))
			{
				if(term!=0)
				{
					puts("\tpush eax");
					x86_terminal(term);
				}
				else 
					x86_stack=x86_stack-4;
				puts("\tpop ecx");
				puts("\tand eax,ecx");
			}
			else
			{
				printf("\tand eax,");
				x86_mem_imm(term);
			}
			break;
		case GEN_DIV+I+4:
				if( (term!=0) && ( (term->type==GEN_NUM+I+4) || (term->type==GEN_ADDRL+P+4) 
									||(term->type==GEN_ADDRF+P+4)||(term->type==GEN_ADDRG+P+4)
									||(term->type==GEN_STRING+P+4)))
				{
					puts("\tpush eax");
					x86_terminal(term);
					puts("\tpop ecx");
					puts("\txchg eax,ecx");
					puts("\tcdq");
					puts("\tidiv ecx");
				}
				else if(term==0)
				{
					x86_stack=x86_stack-4;
					puts("\tpop ecx");
					puts("\tcdq");
					puts("\tidiv ecx");
				}
				else if((term->type==GEN_LOADL+I+4)||(term->type==GEN_LOADF+I+4)||(term->type==GEN_LOADG+I+4))
				{
					puts("\tcdq");
					puts("\tidiv ");
					x86_mem_imm(term);
				}
			break;
		case GEN_EQ+I+4:
		case GEN_NE+I+4:
		case GEN_LT+I+4:
		case GEN_GT+I+4:
		case GEN_LE+I+4:
		case GEN_GE+I+4:
			if((term==0)||(term->type==GEN_ADDRL+P+4)||(term->type==GEN_ADDRF+P+4))
			{
				if(term!=0)
				{
					puts("\tpush eax");
					x86_terminal(term);
				}
				else 
					x86_stack=x86_stack-4;
				puts("\tpop ecx");
				puts("\tcmp eax,ecx");
			}
			else
			{
				printf("\tcmp eax,");
				x86_mem_imm(term);
			}
			switch(op)
			{
				case GEN_EQ+I+4: puts("\tsete al"); 	break;
				case GEN_NE+I+4: puts("\tsetne al"); break;
				case GEN_LT+I+4: puts("\tsetl al"); 	break;
				case GEN_GT+I+4: puts("\tsetg al"); 	break;
				case GEN_LE+I+4: puts("\tsetle al");	break;
				case GEN_GE+I+4: puts("\tsetge al");	break;
			}
			puts("\tmovzx eax,al");
			break;
		case GEN_ASSIGN+I+1:
		case GEN_ASSIGN+I+2:		
		case GEN_ASSIGN+I+4:
		case GEN_ASSIGN+P+4:
		{
			char *acc=op==GEN_ASSIGN+I+1?"al":(op==GEN_ASSIGN+I+2?"ax":"eax");
			if(term==0)
			{
				x86_stack=x86_stack-4;
				puts("\tpop ecx");
				printf("\tmov [eax],%s\n",op==GEN_ASSIGN+I+1?"cl":(op==GEN_ASSIGN+I+2?"cx":"ecx"));
			}
			else if(term->type==GEN_ADDRL+P+4)
			{
				printf("\tmov [ebp-%d],%s\n",(int)term->val+4,acc);
			}
			else if(term->type==GEN_ADDRF+P+4)
			{
				printf("\tmov [ebp+%d],%s\n",(int)term->val*4+8,acc);
			}
			else if(term->type==GEN_ADDRG+P+4)
			{
				printf("\tmov [%s],%s\n",(char *)term->ptr,acc);
			}
			break;
		}
	}
	return;
}

void x86_branch(int op, struct terminal *term, int label)
{
	if((term==0)||(term->type==GEN_ADDRL+P+4)||(term->type==GEN_ADDRF+P+4))
	{
		if(term!=0)
		{
			puts("\tpush eax");
			x86_terminal(term);
		}
		else 
			x86_stack=x86_stack-4;
		puts("\tpop ecx");
		puts("\tcmp eax,ecx");
	}
	else
	{
		printf("\tcmp eax,");
		x86_mem_imm(term);
	}
	switch(op)
	{
		case GEN_JEQ+I+4:	printf("\tje");		break;
		case GEN_JNE+I+4:	printf("\tjne");	break;
		case GEN_JLE+I+4:	printf("\tjle");	break;
		case GEN_JLT+I+4:	printf("\tjl");		break;
		case GEN_JGE+I+4:	printf("\tjge");	break;
		case GEN_JGT+I+4:	printf("\tjg");		break;
	}
	printf(" .L%d\n",label);
	return;
}

int gen_expression(struct astnode *ast);
int resolve_terminal();

void x86_eval_call_arg(struct astlist *arguments)
{
	if(arguments==NULL)
		return;
	x86_eval_call_arg(arguments->next);
	gen_expression(arguments->child);
	resolve_terminal();
	puts("\tpush eax");
	x86_stack=x86_stack+4;
	return;
}

void x86_call(void *lvalue,struct astlist *arguments,int size,int virtual_call)
{
	int arg_size=0;
	for(struct astlist *list=arguments;list!=NULL;list=list->next)
		arg_size=arg_size+4;
	int next_align=x86_stack+arg_size+8;
	for(;next_align>16;next_align=next_align-16);
	int offset=16-next_align;
	//fprintf(stderr,"stack %d\ncall offset %d\n",x86_stack,offset);
	
	printf("\tsub esp,%d\n",offset);
	x86_stack=x86_stack+offset;
	
	x86_eval_call_arg(arguments);
	if(virtual_call)
	{
		gen_expression(lvalue);
		resolve_terminal();
		printf("\tcall eax\n");
	}
	else
		printf("\tcall %s\n",(char *)lvalue);
	if(size==1)
		puts("\tmovsx eax,al");
	else if(size==2)
		puts("\tmovsx eax,ax");
	
	printf("\tadd esp,%d\n",offset+arg_size);
	x86_stack=x86_stack-(offset+arg_size);
	return;
}

void x86_string(int n, char *str)
{
	printf(".S%d:\n",n);
	for(;*str!=0;str=str+1)
		printf("\tdb %d\n",*str);
	puts("\tdb 0\n");
	return;
}

void x86_setup()
{
	x86_gen.extern_global=x86_extern;
	x86_gen.define_global=x86_define;
	x86_gen.common_global=x86_common;
	x86_gen.function_prolog=x86_prolog;
	x86_gen.function_epilog=x86_epilog;
	x86_gen.enter_frame=x86_enter;
	x86_gen.leave_frame=x86_leave;
	x86_gen.function_return=x86_return;
	x86_gen.label=x86_label;
	x86_gen.jump_nz=x86_jnz;
	x86_gen.jump_z=x86_jz;
	x86_gen.jump=x86_jump;
	x86_gen.jump_case=x86_jump_case;
	x86_gen.save_top=x86_save_top;
	x86_gen.load_const=x86_constant;
	x86_gen.terminal_expression=x86_terminal;
	x86_gen.cast_expression=x86_cast_expression;
	x86_gen.unary_expression=x86_unary_expression;
	x86_gen.expression=x86_expression;
	x86_gen.branch=x86_branch;
	x86_gen.call=x86_call;
	x86_gen.string=x86_string;
	interface=&x86_gen;
	
	x86_size.int_size=4;
	x86_size.int_align=4;
	x86_size.short_size=2;
	x86_size.short_align=2;
	x86_size.pointer_size=4;
	x86_size.pointer_align=4;
	x86_size.long_size=4;
	x86_size.long_align=4;
	x86_size.stack_downward=1;
	gen_size=&x86_size;
	
	return;
}