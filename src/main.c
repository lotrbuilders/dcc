#include <stdio.h>
#include "tokens.h"
#include "lexer.h"
#include "type.h"
#include "node.h"
#include "parser.h"

void init_tokens();
struct astnode *check_semantics(struct astnode *ast);
struct astnode *eval(struct astnode *ast);
int gen(struct astnode *ast);
void x86_setup();

int error_count;

int main()
{
	init_tokens();
	x86_setup();
	/*
	int tk;
	while((tk=lex())!=EOF)
	{
		printf("%s\n",token_to_string(tk));
	}*/
	void *ast=parse();
	fputs("\n\n",stderr);
	check_semantics(ast);
	fprintf(stderr,"errorcount %d\n",error_count);
	if(error_count>0)
		return -1;
	fputs("\n\n",stderr);
	struct astnode *ast2=eval(ast);
	gen(ast2);
	return 0;
}