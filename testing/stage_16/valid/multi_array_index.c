int main()
{
	int a[4][4];
	for(int i=0;i<4;i=i+1)
		for(int j=0;j<4;j=j+1)
		a[i][j]=i*j;
	int total=0;
	for(int i=0;i<4;i=i+1)
		for(int j=0;j<4;j=j+1)
			total=total+a[i][j];
	return total;
}