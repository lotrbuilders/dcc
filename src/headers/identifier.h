void enter_block();
int leave_block();
void add_local_id(char *name,struct typelist *type);
int find_local_id(char *name,struct typelist **type);
int id_inblock(char *name);

void add_global_id(char *name,struct typelist *type,char defined);
int is_global(char *name,struct typelist **type);
void define_global(char *name,int define);
int is_defined(char *name);
void remove_globals();

void add_argument_id(char *name,struct typelist *type);
int find_argument_id(char *name,struct typelist **type);
void remove_arguments();