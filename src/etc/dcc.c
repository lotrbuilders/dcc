#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSEMBLE	1
#define COMPILE  	2

#define DCCDIR		"/usr/local/lib/dcc/"
#define TMPDIR		"/tmp/"


char *strdup(char *str);
int strlen(char *str);
char *strcpy(char *s1,char *s2);

char *input_file;
char *cpp_file;
char *asm_file;
char *obj_file;
char *output_file;
char *include_string;
int last_stage;

struct list
{
	struct list *next;
	char *name;
} *include_dir=NULL;

void *smalloc(int size)
{
	void *ptr=calloc(1,size);
	if(ptr==NULL)
	{
		fprintf(stderr,"Fatal Error: Out of memory\n");
		exit(-1);
	}
	return ptr;
}

void process_arguments( int argc, char **argv)
{
	for(int i=1;i<argc;i=i+1)
	{
		if(*argv[i]=='-')
		{
			switch(argv[i][1])
			{
				case 'O': break;
				case 'W': break;
				case 'h': break;
				case 'o': //select output file
					output_file=argv[i+1];
					break;
				case 'I': //select include dir
				{
					struct list *new=smalloc(sizeof(struct list));
					new->name=argv[i]+2;
					new->next=include_dir;
					include_dir=new;
					break;
				}
				case 'S': //output assembly
					last_stage=COMPILE;
					break;
				case 'c': //output object code
					last_stage=ASSEMBLE;
					break;
				case 'm': 
				case 's': break;
				default:
					fprintf(stderr,"Error unknown option\n");
			}
		}
		else 
		{
			input_file=argv[i];
		}
	}
}

char *sanitize_file(char *str,char *file_format)
{
	int seperator=-1;
	int slash=-1;
	if(strlen(str)>120)
	{
		fprintf(stderr,"Error: filename to long\n");
		exit(-1);
	};
	int i;
	for(i=0;str[i]!=0;i=i+1)
	{
		if((str[i]=='/')||(str[i]=='\\'))
			slash=i;
		if(str[i]=='.')
			seperator=i;
	}
	char *out=smalloc(128);
	if(seperator==-1)
		snprintf(out,127,"%s%s.%s",TMPDIR,str+slash+1,file_format);
	else 
	{
		str[seperator]=0;
		snprintf(out,127,"%s%s.%s",TMPDIR,str+slash+1,file_format);
		str[seperator]='.';
	}
	return out;
}

void create_filenames()
{
	cpp_file=sanitize_file(input_file,"ppc");
	
	if(last_stage!=COMPILE||output_file==NULL)
		asm_file=sanitize_file(input_file,"s");
	else 
		asm_file=output_file;
	
	if(last_stage!=ASSEMBLE||output_file==NULL)
		obj_file=sanitize_file(input_file,"o");
	else 
		obj_file=output_file;
	
	if(output_file==NULL)
		output_file="a.out";
	
	return;
}

void create_include_dir()
{
	include_string=smalloc(512);
	sprintf(include_string,"-I%sinclude/",DCCDIR);
	char temp[512];
	for(struct list *lst=include_dir;lst!=NULL;lst=lst->next)
	{
		snprintf(temp,512,"%s -I%s",include_string,lst->name);
		strcpy(include_string,temp);
	}
}

int main(int argc, char **argv)
{
	process_arguments(argc,argv);
	create_filenames();
	create_include_dir();
	int out;
	char command[512];
	
	snprintf(command,512,"cpp -nostdinc %s -o %s %s",include_string,cpp_file,input_file);
	out=system(command);
	if(out)
		exit(-1);
	
	snprintf(command,512,"%s/cc <%s >%s",DCCDIR,cpp_file,asm_file);
	out=system(command);
	if(out)
		exit(-1);
	if(last_stage==COMPILE)
		return 0;
	
	snprintf(command,512,"nasm -felf -F dwarf -g -o %s %s",obj_file,asm_file);
	out=system(command);
	if(out)
		exit(-1);
	if(last_stage==ASSEMBLE)
		return 0;
	
	snprintf(command,512,"cc -m32 -o %s %s",output_file,obj_file);
	out=system(command);
	if(out)
		return -1;
	
	return 0;
} 
