int a=5;
int *foo()
{
	return &a;
}
int main()
{
	return *foo();
}