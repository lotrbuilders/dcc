char *malloc(int size);
int puts(char *s);

int main() {
    char *ptr=malloc(8*4);
	*ptr=65;
	*(ptr+1)=66;
	*(ptr+2)=67;
	*(ptr+3)=68;
	return puts(ptr);
}