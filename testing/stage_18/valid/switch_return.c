int main()
{
	int exp=5;
	int num=5;
	switch(exp)
	{
		case 0:return 0;
		case 1: return num;
		case 2: return num+num;
		default: return exp*num;
	}
}