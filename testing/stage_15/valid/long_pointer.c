char *malloc(int size);

int main()
{
	long *ptr=malloc(3*sizeof(long));
	*ptr=1;
	*(ptr+1)=4;
	*(ptr+2)=9;
	return *(ptr+0)**(ptr+1)**(ptr+2);
}