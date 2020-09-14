int is_terminal(int num)
{
	switch(num)
	{
		case 1+1+2:
		case 2+2+2:
		case 3+3+2:
			return 1;
		default:
			return 0;
	}
	return 0;
}