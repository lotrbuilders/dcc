/*
	Convert tokens to text for errorhandling DCC
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
#include "tokens.h"
char *token_strings[256];
char token_buffer[2];

void init_tokens()
{
	token_strings[TOK_NUMBER-TOK_NUMBER]="number";
	token_strings[TOK_ID-TOK_NUMBER]="identifier";
	token_strings[TOK_STRING-TOK_NUMBER]="string";
	
	token_strings[TOK_INT-TOK_NUMBER]="int";
	token_strings[TOK_CHAR-TOK_NUMBER]="char";
	token_strings[TOK_SHORT-TOK_NUMBER]="short";
	token_strings[TOK_LONG-TOK_NUMBER]="long";
	token_strings[TOK_STRUCT-TOK_NUMBER]="struct";
	token_strings[TOK_VOID-TOK_NUMBER]="void";
	
	token_strings[TOK_RETURN-TOK_NUMBER]="return";
	token_strings[TOK_IF-TOK_NUMBER]="if";
	token_strings[TOK_ELSE-TOK_NUMBER]="else";
	token_strings[TOK_WHILE-TOK_NUMBER]="while";
	token_strings[TOK_DO-TOK_NUMBER]="do";
	token_strings[TOK_FOR-TOK_NUMBER]="for";
	token_strings[TOK_BREAK-TOK_NUMBER]="break";
	token_strings[TOK_CONTINUE-TOK_NUMBER]="continue";
	token_strings[TOK_SWITCH-TOK_NUMBER]="switch";
	token_strings[TOK_CASE-TOK_NUMBER]="case";
	token_strings[TOK_DEFAULT-TOK_NUMBER]="default";
	
	token_strings[TOK_SIZEOF-TOK_NUMBER]="sizeof";
	token_strings[TOK_LAND-TOK_NUMBER]="&&";
	token_strings[TOK_LOR-TOK_NUMBER]="||";
	token_strings[TOK_EQ-TOK_NUMBER]="==";
	token_strings[TOK_NE-TOK_NUMBER]="!=";
	token_strings[TOK_LE-TOK_NUMBER]="<=";
	token_strings[TOK_GE-TOK_NUMBER]=">=";
	token_strings[TOK_ARROW-TOK_NUMBER]="->";
	return;
}

char *token_to_string(int token)
{
	if(token==TOK_EOF)
		return "EOF";
	else if(token<128)
	{
		token_buffer[0]=token;
		token_buffer[1]=0;
		return token_buffer;
	}
	else
		return token_strings[token-TOK_NUMBER];
}