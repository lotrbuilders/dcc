struct test
{
	char c;
	int i;
}

int main()
{
	struct test a;
	a.c=2;
	a.i=5;
	return a.c*a.i;
}