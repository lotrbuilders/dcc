/*
	Eval header DCC
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
#define I 0
#define U 32
#define P 64

#define GEN_LIST -1
#define GEN_FUNCTION 0
#define GEN_EXTERN 1
#define GEN_FUNCTION_PROLOG 10
#define GEN_FUNCTION_EPILOG 11

#define GEN_JUMP 20
#define GEN_LABEL 21
#define GEN_CASE 22

#define GEN_NUM 		0x100

#define GEN_NEG 		0x200
#define GEN_BNOT 		0x300
#define GEN_LNOT 		0x400
#define GEN_MUL			0x500
#define GEN_DIV 		0x600
#define GEN_ADD 		0x700
#define GEN_SUB 		0x800
#define GEN_NE 			0x900
#define GEN_EQ 			0xA00
#define GEN_LE 			0xB00
#define GEN_GE 			0xC00
#define GEN_LT 			0xD00
#define GEN_GT 			0xE00
#define GEN_LAND 		0xF00
#define GEN_LOR 		0x1000
#define GEN_ASSIGN 		0x1100
#define GEN_ADDRL 		0x1200
#define GEN_LOADL 		0x1300
#define GEN_SEL 		0x1400

#define GEN_CALL		0x1500 
#define GEN_ADDRF		0x1600
#define GEN_LOADF		0x1700
#define GEN_ARG			0x1800

#define GEN_ADDRG		0x1900
#define GEN_LOADG		0x1A00
#define GEN_COMMON		0x1B00
#define GEN_GLOBAL		0x1C00

#define GEN_INDIR		0x1D00
#define GEN_STRING		0x1E00
#define GEN_CAST		0x1F00

#define GEN_BAND		0x2000
#define GEN_VCALL		0x2100

#define GEN_JZ			0x4000
#define GEN_JNZ			0x4100
#define GEN_RETURN 		0x4200

#define	GEN_JEQ			0x5000
#define	GEN_JNE			0x5100
#define	GEN_JLT			0x5200
#define	GEN_JLE			0x5300
#define	GEN_JGT			0x5400
#define	GEN_JGE			0x5500

struct target_size
{
	char int_size;
	char int_align;
	char pointer_size;
	char pointer_align;
	char long_size;
	char long_align;
	char short_size;
	char short_align;

	char stack_downward;
};

struct target_size *gen_size;