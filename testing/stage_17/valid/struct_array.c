struct test
{
	char a;
	short b;
	int c;
	long d;
	char *s;
} b[3];

int main()
{
	b[0].b=2;
	b[1].a=1;
	b[2].c=4;
	return b[0].b+b[1].a+b[2].c;
}