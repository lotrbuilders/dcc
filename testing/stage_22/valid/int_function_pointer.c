int printf(char *format);
int hello_world()
{
	return printf("Hello World!\n");;
}

int main()
{
	int (*test)()=&hello_world;
	return test();
}