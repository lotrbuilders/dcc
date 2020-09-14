int *malloc(int size);
int *foo(int *p)
{
	return p+1;
}
int main()
{
	int *p=malloc(32);
	*(p+1)=4;
	return *foo(p);
}