#define TYPE_INT 		0
#define TYPE_CHAR 		1
#define TYPE_SHORT		2
#define TYPE_LONG 		3
#define TYPE_STRUCT 	16
#define TYPE_POINTER 	32
#define TYPE_ARRAY		64
#define TYPE_FUNCTION 	128
#define TYPE_VOID		256


#define TYPE_ID 8192

struct typelist 
{
	void *val;
	char *structname;
	struct typelist *next;
	short type;
};

struct typelist *new_type(short type,void *val,struct typelist* next);
struct typelist *append_type(struct typelist *list,short type,void *val,struct typelist *next);
struct typelist *append_typelist(struct typelist *list1,struct typelist *list2);
struct typelist *replace_typelist(struct typelist *list1,struct typelist *list2,short type);
struct typelist *insert_typelist(struct typelist *type,struct typelist *insert);

char *find_type_identifier(struct typelist *list,int line);
int compare_type(struct typelist *list1,struct typelist *list2);

int target_sizeof(struct typelist *list);

void check_declaration_type(struct typelist *list,int line);
int is_arithmatic_type(struct typelist *list);
int is_returnable_type(struct typelist *list);
int is_compatible_type(struct typelist *list1,struct typelist *list2);
struct typelist *unary_type_promotion(struct typelist *list);
struct typelist *binary_type_promotion(struct typelist *left,struct typelist *right);
int align_type(struct typelist *type,int location);


void print_type(struct typelist *input);

void free_type(struct typelist *list);

int local_array_size(struct typelist *type,int first);