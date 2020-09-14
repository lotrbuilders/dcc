char *calloc(int nmemb,int size);

struct test
{
	int a;
	char b;
	struct test *next;
};

int main()
{
	struct test *ptr=calloc(1,sizeof(struct test));
	ptr->next=calloc(1,sizeof(struct test));
	ptr->next->b=4;
	return ptr->next->b;
}