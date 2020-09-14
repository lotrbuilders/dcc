struct test
{
	char a;
	short b;
	int c;
	long d;
	char *s;
} b;

int main()
{
	b.b=2;
	b.a=1;
	b.c=4;
	return b.b+b.a+b.c;
}