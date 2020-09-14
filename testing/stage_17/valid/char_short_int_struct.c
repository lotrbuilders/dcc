struct test
{
	char a;
	short b;
	int c;
	long d;
	char *s;
};

int main()
{
	struct test b;
	b.b=2;
	b.a=1;
	b.c=4;
	return b.b+b.a+b.c;
}