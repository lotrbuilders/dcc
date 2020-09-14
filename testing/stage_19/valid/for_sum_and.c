int main()
{
	int total=0;
	for(int i=0;i<256;i=i+1)
	{
		total=total+!(i&170);
	}
	return total;
}