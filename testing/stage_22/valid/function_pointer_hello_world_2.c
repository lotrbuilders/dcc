int printf(char *format);
void hello_world()
{
	printf("Hello World!\n");
	return;
}

int main()
{
	void (*test)()=&hello_world;
	test();
	return 34;
}