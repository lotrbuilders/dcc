int a=3;
void *test()
{
	a=a+1;
	return &a;
}

int main()
{
	int *ptr=test();
	return *ptr;
}