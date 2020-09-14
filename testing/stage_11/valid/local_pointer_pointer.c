int main()
{
	int a=4;
	int *b=&a;
	int **c=&b;
	return **c;
}