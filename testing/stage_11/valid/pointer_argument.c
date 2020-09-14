int a=5;
int foo(int *a)
{
	return *a;
}
int main()
{
	return foo(&a);
}