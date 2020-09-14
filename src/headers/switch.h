struct switchlist
{
	struct switchlist *next;
	int val;
	int label;
	char def;
};
int in_switch;

void enter_switch(int line);
struct switchlist *leave_switch();
int add_case(int val);
int add_default();