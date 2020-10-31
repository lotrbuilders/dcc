/*
	 backend DCC
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

struct gen_struct mod5_gen;
struct target_size mod5_size;
int mod5_stack;
int mod5_label_count;

/*
	stack grows from 0xff00 downwards towards 0
		for push stack decrements first then stores data
		for pop data is loaded first then stack is incremented
	%r0	 		is zero register
	%r1  		is compiler register
	%r2  - %r7	are reserved
	%r8  - %r15 are temp registers
	%r16 - %r27 are unused
	%r28 		is return address
	%r29 		is reserved
	%r30 		is frame pointer 
	%r31 		is stack pointer 
*/

//Checks if constant fits in a signed 16-bit immediate
int mod5_is_imm16(int val)
{
	if((val>32767)||(val<-32767))
		return 0;
	return 1;
}

//Provisions for storing constants that do not fit into 16-bits
int mod5_imm32_array[4096];
int mod5_imm32_count;
int  mod5_store_imm32(int val)
{
	if(mod5_imm32_count<4096)
	{
		mod5_imm32_array[mod5_imm32_count]=val;
		mod5_imm32_count=mod5_imm32_count+1;
		return mod5_imm32_count-1;
	}
	fprintf(stderr,"Error: too many constants in program\n");
	return 4096;
}

//Subroutine that loads a register with a constant including size checking
void mod5_load_constant(int reg,int val)
{
	if(mod5_is_imm16(val))
	{
		printf("\tmov %%r%d, %d\n",reg,val);
	}
	else
	{
		int loc=mod5_store_imm32(val);
		printf("\tlw %%r%d, [__C%d]\n",reg,loc);
	}
	return;
}

//Subroutine to push a specific register to the stack
void mod5_push_reg(int reg)
{
	puts("\tsub %r31, 4");
	printf("\tsw [%%r31], %%r%d\n",reg);
	return;
}

//Subroutine to pop a specific register from the stack
void mod5_pop_reg(int reg)
{
	printf("\tlw %%r%d,[%%r31]\n",reg);
	puts("\tadd %r31, 4");
	return;
}

//Code to get new registers for complicated expressions.(Might be ported to x86)
int mod5_current_reg=8;
int mod5_reg_push_count=0;
int mod5_next_reg()
{
	if(mod5_current_reg==15)
	{
		mod5_push_reg(15);
		mod5_reg_push_count=mod5_reg_push_count+1;
		return 15;
	}
	else
	{
		mod5_current_reg=mod5_current_reg+1;
		return mod5_current_reg;
	}
}

//Code to return current register to old register
int mod5_prev_reg()
{
	if(mod5_current_reg==15)
	{
		if(mod5_reg_push_count)
		{
			mod5_pop_reg(1);
			mod5_reg_push_count=mod5_reg_push_count-1;
			return 1;
		}
		else
		{
			return 15;
		}
	}
	else
	{
		mod5_current_reg=mod5_current_reg-1;
		return mod5_current_reg;
	}
}

//Global prolog, will add necessary functions to top of program including jump main
void mod5_global_prolog()
{
	//TODO: use this space to insert library function
	printf("\tlw %%r31,[__STACK_START]\n");
	printf("\tjal %%r28,main\n");
	printf("__INFINITE_LOOP:\n\tjp __INFINITE_LOOP\n");
	printf("__STACK_START:\n\tdw 0xfe00\n");
	printf("wait:\n");
	printf("\tmov %%r1,%%r31\n");
	printf("\tadd %%r1,4\n");
	printf("\tlw %%r2,[%%r1]\n");
	printf("\tmul %%r2,25000\n");
	printf("__WAIT_LOOP:\n");
	printf("\tsub %%r2,1\n");
	printf("\tblt %%r0,%%r2,__WAIT_LOOP\n");
	printf("\tjr %%r28\n");
	return;
}

//Global epilog. Writes 16-bit+ constants to memory
void mod5_global_epilog()
{
	for(int i=0;i<mod5_imm32_count;i=i+1)
	{
		printf("__C%d:\tdw %d\n",i,mod5_imm32_array[i]);
	}
	return;
}

//For extern decleration. Unused due to lack of linker
void mod5_extern(char *name)
{
	return;	
}

//Subroutine to define global variables
void mod5_define(char *name,int size,long val)
{
	printf("%s:\n\td%c %d\n",name,size==1?'b':'w',(int)val);
	return;
}

//Subroutine to add declared global variables
void mod5_common(char *name,int size)
{
	printf("%s:\n\td%c %d\n",name,size==1?'b':'w',0);
	return;
}

//Subroutine to construct function prologue
void mod5_prolog(char *name,struct astnode *arglist)
{
	printf("%s:\n",name);
	mod5_push_reg(30);
	puts("\tmov %r30,%r31");
	return;
}

//Subroutine for epilogue. Goes unused
void mod5_epilog(char *name)
{
	return;
}

//Subroutine to enter new stackframe
void mod5_enter(int size)
{
	if(size!=0)
		printf("\tsub %%r31,%d\n",size);
	mod5_stack=mod5_stack+size;
	return;
}

//Subroutine to leave stackframe
void mod5_leave(int size)
{
	if(size!=0)
		printf("\tadd %%r31,%d\n",size);
	//mod5_stack=0;
	return;
}

//Subroutine which leaves function
void mod5_return(int size)
{
	mod5_pop_reg(30);
	puts("\tjr %r28");
	return;
}

//Subroutine for local numbered labels for jumps and the sort
void mod5_label(int n)
{
	printf("__L%d:\n",n);
	return;
}

//Jump if the current register is not equal to zero
void mod5_jnz(int n)
{
	//puts("\ttest eax,eax");
	printf("\tbne %%r%d, %%r0, __L%d\n",mod5_current_reg,n);
	return;
}

//Jump if the current register is equal to zero
void mod5_jz(int n)
{
	printf("\tbeq %%r%d, %%r0, __L%d\n",mod5_current_reg,n);
	return;
}

//Unconditional jump to label
void mod5_jump(int n)
{
	printf("\tjp __L%d\n",n);
	return;
}

//Switch statement jump for specific value
void mod5_jump_case(int val,int label)
{
	mod5_load_constant(1,val);
	printf("\tbeq %%r8,%%r1,__L%d\n",label);
	return;
}

//Safe old register and switch to new register
void mod5_save_top()
{
	mod5_next_reg();
	return;
}

//Load constant(from generator interface)
void mod5_constant(int n)
{
	mod5_load_constant(mod5_current_reg,n);
	return;
}

//Return wether the submitted terminal, functions as such for this platform
int mod5_is_terminal(struct astnode *ast)
{
	switch(ast->id)
	{
		case GEN_NUM+P+4:
		case GEN_NUM+I+4:
		{
			int val=((struct constnode *)ast)->val;
			return mod5_is_imm16(val);
		}
		case GEN_LOADL+I+4:
		case GEN_LOADL+P+4:
		case GEN_LOADL+I+1:
		case GEN_LOADL+I+2:
		case GEN_LOADF+I+4:
		case GEN_LOADF+P+4:
		case GEN_LOADF+I+1:
		case GEN_LOADF+I+2:
		case GEN_LOADG+I+4:
		case GEN_LOADG+P+4:
		case GEN_LOADG+I+1:
		case GEN_LOADG+I+2:
		case GEN_ADDRL+P+4:
		case GEN_ADDRF+P+4:
			return 0;
			
		case GEN_ADDRG+P+4:
		case GEN_STRING+P+4:
			return 1;
	}
	return 0;
}

//Generate a terminal
void mod5_terminal(struct terminal *term)
{
	if(term==0)
		return;
	switch(term->type)
	{
		case GEN_NUM+P+4:
		case GEN_NUM+I+4:
			mod5_constant(term->val);
			break;
		case GEN_LOADL+I+4:
		case GEN_LOADL+P+4:
			printf("\tmov %%r1,%%r30\n");
			printf("\tsub %%r1,%d\n",(int)term->val+4);
			printf("\tlw %%r%d,[%%r1]\n",mod5_current_reg);
			break;
		case GEN_LOADL+I+1:
			printf("\tmov %%r1,%%r30\n");
			printf("\tsub %%r1,%d\n",(int)term->val+4);
			printf("\tlb %%r%d,[%%r1]\n",mod5_current_reg);
			break;
		case GEN_LOADF+I+4:
		case GEN_LOADF+P+4:
			printf("\tmov %%r1,%%r30\n");
			printf("\tadd %%r1,%d\n",(int)term->val*4+8);
			printf("\tlw %%r%d,[%%r1]\n",mod5_current_reg);
			break;
		case GEN_LOADF+I+1:
			printf("\tmov %%r1,%%r30\n");
			printf("\tadd %%r1,%d\n",(int)term->val*4+8);
			printf("\tlb %%r%d,[%%r1]\n",mod5_current_reg);
			break;
		case GEN_LOADG+I+4:
		case GEN_LOADG+P+4:
			printf("\tlw %%r%d,[%s]\n",mod5_current_reg,(char*)term->ptr);
			break;
		case GEN_LOADG+I+1:
			printf("\tlb %%r%d,[%s]\n",mod5_current_reg,(char*)term->ptr);
			break;
		case GEN_ADDRL+P+4:
			printf("\tmov %%r%d,%%r30\n",mod5_current_reg);
			printf("\tsub %%r%d,%d\n",mod5_current_reg,(int)term->val+4);
			break;
		case GEN_ADDRF+P+4:
			printf("\tmov %%r%d,%%r30\n",mod5_current_reg);
			printf("\tadd %%r%d,%d\n",mod5_current_reg,(int)term->val*4+8);
			break;
		case GEN_ADDRG+P+4:
			printf("\tmov %%r%d,%s\n",mod5_current_reg,(char*)term->ptr);
			break;
		case GEN_STRING+P+4:
			printf("\tmov %%r%d,__S%d\n",mod5_current_reg,(int)term->val);
			break;
	}
	return;
}

//Generate a unary_expression
void mod5_unary_expression(int op,struct terminal *term)
{
	switch(op)
	{
		case GEN_NEG+I+4:
			mod5_terminal(term);
			printf("\txor %%r%d,-1\n",mod5_current_reg);
			printf("\tadd %%r%d,1\n",mod5_current_reg);
			break;
			
		case GEN_BNOT+I+4:
			mod5_terminal(term);
			printf("\txor %%r%d,-1\n",mod5_current_reg);
			break;
		
		case GEN_LNOT+I+4:
			mod5_terminal(term);
			printf("\tbeq %%r%d, %%r0, __M%d\n",mod5_current_reg,mod5_label_count);
			printf("\tmov %%r%d,%%r0\n",mod5_current_reg);
			printf("\tjp __M%d\n",mod5_current_reg+1);
			printf("__M%d:\n",mod5_label_count);
			printf("\tmov %%r%d,1\n",mod5_current_reg);
			printf("__M%d:\n",mod5_label_count+1);
			mod5_label_count=mod5_label_count+2;
			break;
		case GEN_INDIR+I+4:
		case GEN_INDIR+P+4:
			mod5_terminal(term);
			printf("\tlw %%r%d,[%%r%d]\n",mod5_current_reg,mod5_current_reg);
			break;
		case GEN_INDIR+I+1:
			mod5_terminal(term);
			printf("\tlb %%r%d,[%%r%d]\n",mod5_current_reg,mod5_current_reg);
			break;
	}
	return;
}

//Cast expression(Only 4 bytes to 1 byte is interesting. Done by push/pop due)
void mod5_cast_expression(int to,int from)
{
	if(to<from)
	{
		if(to==1)
		{
			mod5_push_reg(mod5_current_reg);
			printf("\tlb %%r%d,[%%r31]\n",mod5_current_reg);
			puts("\tadd %r31, 4");
		}
	}
	return;
}

//Concatenate terminal, which fits in instruction
void mod5_mem_imm(struct terminal *term)
{
	if(term==0)
		return;
	switch(term->type)
	{
		case GEN_NUM+P+4:
		case GEN_NUM+I+4:
			printf("%d\n",(int)term->val);
			break;
		case GEN_ADDRG+P+4:
			printf("%s\n",(char*)term->ptr);
			break;
		case GEN_STRING+P+4:
			printf("__S%d\n",(int)term->val);
			break;
		default:
			printf("unkown mem imm %x\n",term->type);
	}
	return;
}

//Generate binary expression
void mod5_expression(int op,struct terminal *term)
{
	switch(op)
	{
		//Generate addition
		case GEN_ADD+I+4:
		case GEN_ADD+P+4:
			if(term==0)
			{
				int left=mod5_current_reg;
				int right=mod5_prev_reg();
				printf("\tadd %%r%d, %%r%d\n",right,left);
			}
			else
			{
				printf("\tadd %%r%d,",mod5_current_reg);
				mod5_mem_imm(term);
			}
			break;
		//Generate subtraction	
		case GEN_SUB+I+4:
		case GEN_SUB+P+4:
			if(term==0)
			{
				int left=mod5_current_reg;
				int right=mod5_prev_reg();
				printf("\tsub %%r%d, %%r%d\n",right,left);
			}
			else
			{
				printf("\tsub %%r%d,",mod5_current_reg);
				mod5_mem_imm(term);
			}
			break;
		//Generate multiplication
		case GEN_MUL+I+4:
			if(term==0)
			{
				int left=mod5_current_reg;
				int right=mod5_prev_reg();
				printf("\tmul %%r%d, %%r%d\n",right,left);
			}
			else
			{
				printf("\tmul %%r%d,",mod5_current_reg);
				mod5_mem_imm(term);
			}
			break;
		//Generate binary and
		case GEN_BAND+I+4:
			if(term==0)
			{
				int left=mod5_current_reg;
				int right=mod5_prev_reg();
				printf("\tand %%r%d, %%r%d\n",right,left);
			}
			else
			{
				printf("\tand %%r%d,",mod5_current_reg);
				mod5_mem_imm(term);
			}
			break;
		//Crash at division
		case GEN_DIV+I+4:
				fprintf(stderr,"Error: division currently not supported\n");
			break;
		//Generate set if comparison is true
		case GEN_EQ+I+4:
		case GEN_NE+I+4:
		case GEN_LT+I+4:
		case GEN_GT+I+4:
		case GEN_LE+I+4:
		case GEN_GE+I+4:
		{
			int left=mod5_current_reg;
			int right=0;
			if(term==0)
			{
				right=mod5_prev_reg();	
			}
			else
			{
				printf("\tmov %%r1, ");
				mod5_mem_imm(term);
				right=1;
			}
			switch(op)
			{
				case GEN_EQ+I+4:	printf("\tbeq %%r%d, %%r%d, __M%d\n",left,right,mod5_label_count);	break;
				case GEN_NE+I+4:	printf("\tbne %%r%d, %%r%d, __M%d\n",left,right,mod5_label_count);	break;
				case GEN_LT+I+4:	printf("\tblt %%r%d, %%r%d, __M%d\n",left,right,mod5_label_count);	break;
				
				case GEN_LE+I+4:	printf("\tblt %%r%d, %%r%d, __M%d\n",left,right,mod5_label_count);
									printf("\tbeq %%r%d, %%r%d, __M%d\n",left,right,mod5_label_count);	break;
				case GEN_GE+I+4:	printf("\tblt %%r%d, %%r%d, __M%d\n",right,left,mod5_label_count);	break;
				case GEN_GT+I+4:	printf("\tblt %%r%d, %%r%d, __M%d\n",left,right,mod5_label_count);
									printf("\tbne %%r%d, %%r%d, __M%d\n",left,right,mod5_label_count+1);	
									printf("__M%d:\n",mod5_label_count);
									mod5_label_count=mod5_label_count+1;								break;
			}
			printf("\tmov %%r%d,%%r0\n",mod5_current_reg);
			printf("\tjp __M%d\n",mod5_current_reg+1);
			printf("__M%d:\n",mod5_label_count);
			printf("\tmov %%r%d,1\n",mod5_current_reg);
			printf("__M%d:\n",mod5_label_count+1);
			mod5_label_count=mod5_label_count+2;
			break;
		}
		//Generate Assignment statement
		case GEN_ASSIGN+I+1:
		case GEN_ASSIGN+I+2:		
		case GEN_ASSIGN+I+4:
		case GEN_ASSIGN+P+4:
		{
			char *ins=op==GEN_ASSIGN+I+1?"sb":"sw";
			if(term==0)
			{
				int left=mod5_current_reg;
				int right=mod5_prev_reg();
				printf("\t%s [%%r%d],%%r%d\n",ins,left,right);
			}
			else if(term->type==GEN_ADDRL+P+4)
			{
				//printf("\tmov [ebp-%d],%s\n",(int)term->val+4,acc);
				printf("\tmov %%r1,%%r30\n");
				printf("\tsub %%r1,%d\n",(int)term->val+4);
				printf("\t%s [%%r1],%%r%d\n",ins,mod5_current_reg);
			}
			else if(term->type==GEN_ADDRF+P+4)
			{
				printf("\tmov %%r1,%%r30\n");
				printf("\tadd %%r1,%d\n",(int)term->val*4+8);
				printf("\t%s [%%r1],%%r%d\n",ins,mod5_current_reg);
			}
			else if(term->type==GEN_ADDRG+P+4)
			{
				printf("\t%s [%s],%%r%d\n",ins,(char *)term->ptr,mod5_current_reg);
			}
			break;
		}
	}
	return;
}

//Generate conditional branching at statement
void mod5_branch(int op, struct terminal *term, int label)
{
	int left=mod5_current_reg;
	int right=0;
	if(term==0)
	{
		right=mod5_prev_reg();	
	}
	else
	{
		printf("\tmov %%r1, ");
		mod5_mem_imm(term);
		right=1;
	}
	switch(op)
	{
		case GEN_JEQ+I+4:	printf("\tbeq %%r%d, %%r%d, __L%d\n",left,right,label);	break;
		case GEN_JNE+I+4:	printf("\tbne %%r%d, %%r%d, __L%d\n",left,right,label);	break;
		case GEN_JLT+I+4:	printf("\tblt %%r%d, %%r%d, __L%d\n",left,right,label);	break;
		
		case GEN_JLE+I+4:	printf("\tblt %%r%d, %%r%d, __L%d\n",left,right,label);
							printf("\tbeq %%r%d, %%r%d, __L%d\n",left,right,label);	break;
		case GEN_JGE+I+4:	printf("\tblt %%r%d, %%r%d, __L%d\n",right,left,label);	
							printf("\tbeq %%r%d, %%r%d, __L%d\n",right,left,label);	break;
		case GEN_JGT+I+4:	printf("\tblt %%r%d, %%r%d, __L%d\n",right,left,label); break;
	}
	return;
}

int gen_expression(struct astnode *ast);
int resolve_terminal();

//Safe used registers
void mod5_safe_reg_call(int curreg)
{
	for(int i=8;i<=curreg;i=i+1)
		mod5_push_reg(i);
	
	return;
}

//Reload safed registers
void mod5_return_reg_call(int curreg)
{
	
	for(int i=curreg;i>=8;i=i-1)
		mod5_pop_reg(i);
	return;
}

//Evaluate arguments from right to left
int mod5_eval_call_arg(struct astlist *arguments)
{
	if(arguments==NULL)
		return 0;
	int argc=mod5_eval_call_arg(arguments->next);
	gen_expression(arguments->child);
	resolve_terminal();
	mod5_push_reg(mod5_current_reg);
	mod5_stack=mod5_stack+4;
	return argc+1;
}

//Generate call instructions. Backend is responsible for actually generating the arguments
void mod5_call(void *lvalue,struct astlist *arguments,int size,int virtual_call)
{
	mod5_safe_reg_call(mod5_current_reg-1);
	int argc=mod5_eval_call_arg(arguments);
	if(virtual_call)
	{
		gen_expression(lvalue);
		resolve_terminal();
		
		mod5_push_reg(28);
		printf("\tjalr %%r28,%%r%d\n",mod5_current_reg);
		if(mod5_current_reg!=8)
			printf("\tmov %%r%d, %%r8\n",mod5_current_reg);
		mod5_pop_reg(28);
	}
	else
	{
		mod5_push_reg(28);
		printf("\tjal %%r28, %s\n",(char *)lvalue);
		if(mod5_current_reg!=8)
			printf("\tmov %%r%d, %%r8\n",mod5_current_reg);
		mod5_pop_reg(28);
	}
	printf("\tadd %%r31,%d\n",4*argc);
	mod5_return_reg_call(mod5_current_reg-1);
	mod5_cast_expression(size,4);
	return;
}

//Insert constant string into code
void mod5_string(int n, char *str)
{
	printf("__S%d:\n",n);
	for(;*str!=0;str=str+1)
		printf("\tdb %d\n",*str);
	puts("\tdb 0\n");
	return;
}

//Setup all function pointers
void setup_backend()
{
	mod5_gen.global_prolog=mod5_global_prolog;
	mod5_gen.global_epilog=mod5_global_epilog;
	mod5_gen.extern_global=mod5_extern;
	mod5_gen.define_global=mod5_define;
	mod5_gen.common_global=mod5_common;
	mod5_gen.function_prolog=mod5_prolog;
	mod5_gen.function_epilog=mod5_epilog;
	mod5_gen.enter_frame=mod5_enter;
	mod5_gen.leave_frame=mod5_leave;
	mod5_gen.function_return=mod5_return;
	mod5_gen.label=mod5_label;
	mod5_gen.jump_nz=mod5_jnz;
	mod5_gen.jump_z=mod5_jz;
	mod5_gen.jump=mod5_jump;
	mod5_gen.jump_case=mod5_jump_case;
	mod5_gen.save_top=mod5_save_top;
	mod5_gen.load_const=mod5_constant;
	mod5_gen.is_terminal=mod5_is_terminal;
	mod5_gen.terminal_expression=mod5_terminal;
	mod5_gen.cast_expression=mod5_cast_expression;
	mod5_gen.unary_expression=mod5_unary_expression;
	mod5_gen.expression=mod5_expression;
	mod5_gen.branch=mod5_branch;
	mod5_gen.call=mod5_call;
	mod5_gen.string=mod5_string;
	interface=&mod5_gen;
	
	//sizeof(short)==sizeof(int)==sizeof(long)==sizeof(void *)==4
	mod5_size.int_size=4;
	mod5_size.int_align=4;
	mod5_size.short_size=4;
	mod5_size.short_align=4;
	mod5_size.pointer_size=4;
	mod5_size.pointer_align=4;
	mod5_size.long_size=4;
	mod5_size.long_align=4;
	mod5_size.stack_downward=1;
	gen_size=&mod5_size;
	
	return;
}