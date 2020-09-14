char *calloc(int n,int size);

struct test
{
	char c;
	int i;
	short s;
};


int main()
{
	struct test b;
	struct test a;
	char g=4;
	a.c=2;
	a.i=5;
	b.c=a.c+a.i;
	b.i=20;
	return a.c*b.c+a.i*b.i+g;
}