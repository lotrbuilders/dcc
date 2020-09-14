int main()
{
	int a[-2];
	for(int i=0;i<10;i=i+1)
		a[i]=i;
	int total=test(a);
	for(int i=0;i<10;i=i+1)
		total=total+a[i];
	return total;
}