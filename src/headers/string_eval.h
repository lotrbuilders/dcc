int str_count;
struct string_list
{
	void *next;
	char *str;
	int n;
};
struct string_list *local_strings;
int add_string(char *str);