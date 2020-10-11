/*
	Token enumeration header DCC
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
#define TOK_EOF 		-1

#define TOK_NUMBER 		128
#define TOK_ID 			129
#define TOK_STRING		130

#define TOK_INT 		144
#define TOK_CHAR		145
#define TOK_SHORT		146
#define TOK_LONG		147
#define TOK_STRUCT		148
#define TOK_VOID		149

#define TOK_RETURN 		160
#define TOK_IF			161
#define TOK_ELSE		162
#define TOK_WHILE		163
#define TOK_DO			164
#define TOK_FOR			165
#define TOK_BREAK		166
#define TOK_CONTINUE	167
#define TOK_SWITCH		168
#define TOK_CASE		169
#define TOK_DEFAULT		170

#define TOK_SIZEOF		180

#define TOK_LAND		192
#define TOK_LOR			193
#define TOK_EQ			194
#define TOK_NE			195
#define TOK_LE			196
#define TOK_GE			197
#define TOK_ARROW		198

char *token_to_string(int token);