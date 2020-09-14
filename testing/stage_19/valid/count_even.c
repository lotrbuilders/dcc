int main()
{
	int total=0;
	for(int i=0;i<53;i=i+1)
	{
		total=total+!(i&1);
	}
	return total;
}