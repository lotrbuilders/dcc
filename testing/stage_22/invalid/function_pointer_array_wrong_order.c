
int func1(int d)
{
	return d+d;
}
int func2(int d)
{
	return d*d;
}

int main()
{
	int (*test)[2](int d);
	test[0]=func1;
	test[1]=func2;
	return test[0](1)+test[1](2);
}