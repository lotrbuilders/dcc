int main()
{
	int exp=5;
	int num=5;
	int result;
	switch(exp)
	{
		case 0: result=0;break;
		case 1: result=num;break;
		case 2: result=num+num;break;
		default: result=exp*num;break;
	}
	return result;
}