int puts(char *str);
int (*test())(char *ptr)
{
	return puts;
}

int main()
{
	return test()("Hello");
}