int *malloc(int size);


int main() {
    int *ptr=malloc(8*4);
	*ptr=2;
	*(ptr+1)=3;
	*(ptr+2)=4;
	*(ptr+3)=6;
	return *ptr+*(ptr+1)+*(ptr+2)+*(ptr+3);
}