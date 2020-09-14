char *calloc(int n,int size);

struct test
{
	char c;
	int *p;
};

int main()
{
	int a=4;
	struct test *ptr=calloc(sizeof(struct test),1);
	ptr->c=3;
	ptr->p=&a;
	return *ptr->p+ptr->c;
}