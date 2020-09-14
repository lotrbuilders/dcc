int test(int a[10])
{
	for(int i=0;i<10;i=i+1)
		a[i]=i;
	return a[9];
}

int main()
{
	int a[10];
	int total=test(a);
	for(int i=0;i<10;i=i+1)
		total=total+a[i];
	return total;
}