struct astnode
{
	int id;
	int line;
	struct typelist *type;
	
	int child_count;
	void *child[1];
};

struct constnode
{
	int id;
	int line;
	void *type;
	
	long val;
};

struct astlist
{
	int id;
	int line;
	void *type;
	
	void *child;
	void *next;
};

struct astnode *newnode(int id,int line_number,int child_count);

struct constnode *newconst(int id,int line_number,long val);

struct astlist *newastlist(int id,int line_number,void *child,void *next);