/*
	Lexer DCC
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
#include <ctype.h>
#include <string.h>
#include "tokens.h"
#include "lexer.h"
#include "errorhandling.h"

int strcmp(char *s1,char *s2);

int lex_num(int c);
int lex_identifier(int c);
int lex_punctuation(int c);
int lex_string(int c);
int lex_char(int c);

int line_number=1;

int popc()
{
	return fgetc(stdin);
}

int peekc()
{
	int c=fgetc(stdin);
	ungetc(c,stdin);
	return c;
}

int lex()
{
	for(int c=peekc();;c=peekc())
	{
		if(c=='\n')
		{
			line_number=line_number+1;
			popc();
		}
		else if(isspace(c)) 
			popc();
		else if(isalpha(c)||c=='_')
			return lex_identifier(c);
		else if(isdigit(c))
			return lex_num(c);
		else if(c=='"')
			return lex_string(c);
		else if(c=='\'')
			return lex_char(c);
		else if(c=='#')
		{
			popc();
			fscanf(stdin,"%d",&line_number);
			//line_number+1;
			while(popc()!='\n');
		}
		else if(ispunct(c))
			return lex_punctuation(c);
		else if(c==EOF)
			return TOK_EOF;
		else
			lexer_error("Unkown character '%c' during lexing",popc(),line_number);
	}
}

int lex_num(int c)
{
	long i=0;
	if(c=='0')
	{
		popc();
		c=peekc();
		if((c=='x')||c=='X')
		{
			fprintf(stderr,"hex\n");
			popc();
			c=peekc();
			for(i=0;isxdigit(c);c=peekc())
			{
				popc();
				if(isdigit(c))
					i=i*16+c-'0';
				else 
					i=i*16+tolower(c)+10-'a';
			}
		}
		else if(isdigit(c))
		{
			for(i=0;isdigit(c)&&(c<'8');c=peekc())
			{
				popc();
				i=i*8+c-'0';
			}
		}
	}
	else 
	{
		for(i=0;isdigit(c);c=peekc())
		{
			popc();
			i=i*10+c-'0';
		}
	}
	lex_number=i;
	return TOK_NUMBER;
}

int lex_punctuation(int c)
{
	popc();
	switch(c)
	{
		case ';' : 
		case '(' : 
		case ')' : 
		case '{' : 
		case '}' :
		case ']' :
		case '[' :
		
		case '~' :
		case ':' :
		case '?' :
		case '+' :
		case '*' :
		case '/' :
		case ',' :
		case '.' :
			return c;
		case '|' : if(peekc()=='|')	{popc();return TOK_LOR;}	else return c;
		case '&' : if(peekc()=='&')	{popc();return TOK_LAND;}	else return c;
		case '=' : if(peekc()=='=')	{popc();return TOK_EQ;}		else return c;
		case '!' : if(peekc()=='=')	{popc();return TOK_NE;}		else return c;
		case '<' : if(peekc()=='=')	{popc();return TOK_LE;}		else return c;
		case '>' : if(peekc()=='=')	{popc();return TOK_GE;}		else return c;
		case '-' : if(peekc()=='>')	{popc();return TOK_ARROW;} 	else return c;
		default :
			lexer_error("Unkown character '%c' during lexing",c,line_number);
			return ';';
	}
	return TOK_EOF;
}

int lex_identifier(int c)
{
	int i;
	for(i=0 ; (i<127) && ( (isalnum(c)) || (c=='_') ) ; i=i+1)
	{
		lex_buffer[i]=c;
		popc();
		c=peekc();
	}
	lex_buffer[i]=0;
	if(strcmp(lex_buffer,"int")==0)
		return TOK_INT;
	else if(strcmp(lex_buffer,"if")==0)
		return TOK_IF;
	else if(strcmp(lex_buffer,"else")==0)
		return TOK_ELSE;
	else if(strcmp(lex_buffer,"char")==0)
		return TOK_CHAR;
	else if(strcmp(lex_buffer,"for")==0)
		return TOK_FOR;
	else if(strcmp(lex_buffer,"while")==0)
		return TOK_WHILE;
	else if(strcmp(lex_buffer,"return")==0)
		return TOK_RETURN;
	else if(strcmp(lex_buffer,"sizeof")==0)
		return TOK_SIZEOF;
	else if(strcmp(lex_buffer,"long")==0)
		return TOK_LONG;
	else if(strcmp(lex_buffer,"short")==0)
		return TOK_SHORT;
	else if(strcmp(lex_buffer,"struct")==0)
		return TOK_STRUCT;
	else if(strcmp(lex_buffer,"switch")==0)
		return TOK_SWITCH;
	else if(strcmp(lex_buffer,"case")==0)
		return TOK_CASE;
	else if(strcmp(lex_buffer,"default")==0)
		return TOK_DEFAULT;
	else if(strcmp(lex_buffer,"do")==0)
		return TOK_DO;
	else if(strcmp(lex_buffer,"break")==0)
		return TOK_BREAK;
	else if(strcmp(lex_buffer,"continue")==0)
		return TOK_CONTINUE;
	else if(strcmp(lex_buffer,"void")==0)
		return TOK_VOID;
	else 
		return TOK_ID;
}

int lex_string(int c)
{
	popc();
	c=peekc();
	int i;
	for(i=0 ; (i<127) && (c!='"') && (c!='\n') ; i=i+1)
	{
		if(c=='\\')
		{
			popc();
			c=popc();
			switch(c)
			{
				case 'n': lex_buffer[i]='\n'; break;
				case 't': lex_buffer[i]='\t'; break;
				default:
					lexer_error("Unexpected espcape character '\\%C' in string",c,line_number);
				case '\\':
				case '"':
				case '\'':
					lex_buffer[i]=c;
					break;
			}
			
		}
		else 
		{
			lex_buffer[i]=c;
			popc();
		}
		c=peekc();
	}
	if(c=='\n')
		error("Badly terminated string",line_number);
	popc();
	lex_buffer[i]=0;
	return TOK_STRING;
}

int lex_char(int c)
{
	popc();
	c=peekc();
	if(c=='\\')
	{
		popc();
		c=popc();
		switch(c)
		{
			case 'n': lex_number='\n'; break;
			case 't': lex_number='\t'; break;
			default:
				lexer_error("Unexpected espcape character '\\%C' in character constant",line_number,c);
			case '\\':
			case '"':
			case '\'':
				lex_number=c;
				break;
		}
		
	}
	else 
	{
		lex_number=c;
		popc();
	}
	if(peekc()!='\'')
		lexer_error("Expected terminating '\'' in character constant, not '%C'",line_number,peekc());
	popc();
	return TOK_NUMBER;
}