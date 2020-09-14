int a[10];
int test()[10]
{
	return a;
}

int main()
{
	for(int i=0;i<10;i=i+1)
		test()[i]=i;
	int total=test(a);
	for(int i=0;i<10;i=i+1)
		total=total+a[i];
	return total;
}