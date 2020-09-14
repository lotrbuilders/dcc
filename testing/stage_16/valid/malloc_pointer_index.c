char *malloc(int size);
int main()
{
	int *p=malloc(10*sizeof(int));
	for(int i=0;i<10;i=i+1)
		p[i]=i;
	int total=0;
	for(int i=0;i<10;i=i+1)
		total=total+p[i];
	return total;
}