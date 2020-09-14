int **c;
int a=4;
int main()
{
	int *b=&a;
	c=&b;
	return **c;
}