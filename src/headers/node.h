/*
	AST nodes header DCC
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

struct astnode
{
	int id;
	int line;
	struct typelist *type;
	
	int child_count;
	void *child[1];
};

struct constnode
{
	int id;
	int line;
	void *type;
	
	long val;
};

struct astlist
{
	int id;
	int line;
	void *type;
	
	void *child;
	void *next;
};

struct astnode *newnode(int id,int line_number,int child_count);

struct constnode *newconst(int id,int line_number,long val);

struct astlist *newastlist(int id,int line_number,void *child,void *next);