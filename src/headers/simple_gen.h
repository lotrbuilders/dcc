/*
	Interface for simple stack based generator header DCC
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
struct terminal 
{
	void *ptr;
	long val;
	int type;
};

struct gen_struct
{
	void (*extern_global)(char *name);
	void (*common_global)(char *name,int size);
	void (*define_global)(char *name,int size,long val);
	void (*global_prolog)();
	void (*global_epilog)();
	void (*function_prolog)(char *name,struct astnode *arguments);
	void (*function_epilog)(char *name);
	void (*enter_frame)(int size);
	void (*leave_frame)(int size);
	void (*function_return)(int size);
	void (*label)(int n);
	void (*jump_nz)(int n);
	void (*jump_z)(int n);
	void (*jump)(int n);
	void (*jump_case)(int val,int label);
	void (*save_top)();
	void (*load_const)(int n);
	void (*load_string)(int n);
	int  (*is_terminal)(struct astnode *term);
	void (*terminal_expression)(struct terminal *term);
	void (*unary_expression)(int op,struct terminal *term);
	void (*cast_expression)(int to,int from);
	void (*expression)(int op,struct terminal *term);
	void (*branch)(int op, struct terminal *term, int label);
	void (*call)(void *lvalue,struct astlist *arguments,int size,int virtual_call);
	void (*string)(int n,char *str);
};

struct gen_struct *interface;