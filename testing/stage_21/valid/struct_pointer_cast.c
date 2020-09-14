void *malloc(int size);
struct test{ int a; short b; };

void *tree(int a, int b)
{
	struct test *ptr=malloc(sizeof(struct test));
	ptr->a=a;
	ptr->b=b;
	return ptr;
}

int main()
{
	void *ptr=tree(5,16);
	return ((struct test *)ptr)->a+((struct test *)ptr)->b;
}