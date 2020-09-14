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
	void (*terminal_expression)(struct terminal *term);
	void (*unary_expression)(int op,struct terminal *term);
	void (*cast_expression)(int to,int from);
	void (*expression)(int op,struct terminal *term);
	void (*call)(void *lvalue,struct astlist *arguments,int size,int virtual_call);
	void (*string)(int n,char *str);
};

struct gen_struct *interface;