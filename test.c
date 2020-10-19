int main(int argc)
{
	if(argc==0)
	{
		return 1;
	}
	else 
	{
		return argc+(2+main(argc-1));
	}
}