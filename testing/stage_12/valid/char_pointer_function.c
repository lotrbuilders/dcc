char *malloc(int size);
char *c()
{
	char *str=malloc(3);
	for(int i=0;i<3;i=i+1)
		*(str+i)=i*i;
	return str;
}

int main() 
{
	char t=0;
	char *p=c();
	for(int i=0;i<3;i=i+1)
		t=t+*(p+i);
	return t;
}