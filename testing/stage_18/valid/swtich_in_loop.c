int main()
{
	int result=0;
	for(int i=0;i<5;i=i+1)
	{
		switch(i)
		{
			case 0: result=result+0;
			case 1: result=result+2;
			case 2: result=result+3;
			default: result=result+4;
		}
	}
	return result;
}