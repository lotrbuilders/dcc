void *malloc(int size);
void free(void *ptr);

int main()
{
	int *ptr=malloc(2*sizeof(int));
	ptr[0]=4;
	ptr[1]=4+ptr[0];
	int total=ptr[0]+ptr[1];
	free(ptr);
	return total;
}