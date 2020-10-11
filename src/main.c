/*
	Main file DCC
    Copyright (C) 2020  Daan Oosterveld
    DCC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    DCC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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