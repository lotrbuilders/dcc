int printf(char *format);
int hello_world(int d)
{
	int total=0;
	for(int i=0;i<d;i=i+1)
		total=total+printf("Hello World!\n");
	return total;
}

int main()
{
	int (*test)(int d)=&hello_world;
	return test(3);
}