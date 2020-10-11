/*
	Identifier list header DCC
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