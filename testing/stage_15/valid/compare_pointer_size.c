int main()
{
	return sizeof(char*)==sizeof(short *)&&sizeof(short *)==sizeof(int *)&&sizeof (int *)==sizeof(long *)&&(sizeof(long *))==sizeof(int **);
}