int struct_is_defined(char *name);
void define_struct(char *name,void *definition,int line);
struct typelist *find_struct_member(char *structname,char *member_name,int *offset);
int find_struct_size(char *name);
int find_first_member_size(char *name);