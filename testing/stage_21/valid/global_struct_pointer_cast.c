void *malloc(int size);
struct test{ int a; short b; };
void *p;

void tree(int a, int b)
{
	p=malloc(sizeof(struct test));
	((struct test*)p)->a=a;
	((struct test*)p)->b=b;
	return;
}

int main()
{
	tree(5,16);
	return ((struct test *)p)->a+((struct test *)p)->b;
}