struct test
{
	char a[8];
} b;

int main()
{
	for(int i=0;i<8;i=i+1)
		b.a[i]=i*i;
	int total=0;
	for(int i=0;i<8;i=i+1)
		total=total+b.a[i];
	return total;
}