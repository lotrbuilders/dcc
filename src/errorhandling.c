#include <stdio.h>
#include <stdlib.h>
#include "errorhandling.h"
#include "tokens.h"

int error_count;

void lexer_error(char *str,int c,int linenumber)
{
	error_count=error_count+1;
	fputs("Error: ",stderr);
	while (*str!=0)
	{
		if(*str!='%')
			fputc(*str,stderr);
		else
		{
			str=str+1;
			switch(c)
			{
				case '\n':
					fputs("\\n",stderr);
					break;
				case '\t':
					fputs("\\t",stderr);
					break;
				default :
					fputc(c,stderr);
			}
		}
		str=str+1;
	}
	fprintf(stderr," on line %d\n",linenumber);
	if(error_count>20)
		exit(-1);
	return;
}

void parser_error(char *str,int tk,int line)
{
	error_count=error_count+1;
	fputs("Error: ",stderr);
	while (*str!=0)
	{
		if(*str!='%')
			fputc(*str,stderr);
		else
		{
			str=str+1;
			fputs(token_to_string(tk),stderr);
		}
		str=str+1;
	}
	fprintf(stderr," on line %d\n",line);
	if(error_count>20)
		exit(-1);
	return;
}

void error(char *str,int line)
{
	error_count=error_count+1;
	fprintf(stderr,"Error: %s on line %d\n",str,line);
	if(error_count>20)
		exit(-1);
	return;
}