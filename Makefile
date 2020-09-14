IDIR = ./src/headers
BDIR ?= ./obj
DCC ?= dcc
CFLAGS = -I$(IDIR) -m32 -std=c99 -Wall -Werror -O1 -Wno-builtin-declaration-mismatch

vpath %.c ./src 
vpath %parser.c ./src/parser
vpath %gen.c ./src/simple_gen
vpath %.c ./src/optimization 
vpath %.c ./src/etc
vpath %.h $(IDIR)
vpath %.o $(BDIR)

_OBJ = main.o errorhandling.o tokens.o lexer.o node.o parser.o declaration_parser.o statement_parser.o expression_parser.o semantics.o \
		type.o eval.o simple_gen.o identifier.o global_eval.o constant_evaluation.o struct.o switch.o \
		x86_gen.o
OBJ = $(patsubst %,$(BDIR)/%,$(_OBJ))

all: cc dcc

$(BDIR)/errorhandling.o : errorhandling.c
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/node.o : node.c
	$(DCC) $(CFLAGS) -c -o $@ $<


$(BDIR)/tokens.o : tokens.c tokens.h
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/type.o : type.c type.h errorhandling.h 
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/identifier.o : identifier.c type.h errorhandling.h identifier.h 
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/struct.o : struct.c type.h errorhandling.h identifier.h 
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/switch.o : switch.c type.h errorhandling.h identifier.h 
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/global_eval.o : global_eval.c tokens.h errorhandling.h node.h symbols.h type.h 
	$(DCC) $(CFLAGS) -c -o $@ $<
	
	
$(BDIR)/lexer.o :  lexer.c errorhandling.h 
	$(DCC) $(CFLAGS) -c -o $@ $<
	
	
$(BDIR)/parser.o : parser.c lexer.h tokens.h errorhandling.h node.h symbols.h
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/declaration_parser.o : declaration_parser.c tokens.h errorhandling.h node.h symbols.h type.h
	$(DCC) $(CFLAGS) -c -o $@ $<
	
$(BDIR)/statement_parser.o : statement_parser.c tokens.h errorhandling.h node.h symbols.h
	$(DCC) $(CFLAGS) -c -o $@ $<	

$(BDIR)/expression_parser.o : expression_parser.c tokens.h errorhandling.h node.h symbols.h
	$(DCC) $(CFLAGS) -c -o $@ $<
	
	
$(BDIR)/semantics.o : semantics.c type.h node.h symbols.h errorhandling.h
	$(DCC) $(CFLAGS) -c -o $@ $<
	
	
	
$(BDIR)/constant_evaluation.o : constant_evaluation.c type.h node.h symbols.h errorhandling.h eval.h
	$(DCC) $(CFLAGS) -c -o $@ $<

		

$(BDIR)/eval.o : eval.c type.h node.h symbols.h errorhandling.h eval.h
	$(DCC) $(CFLAGS) -c -o $@ $<
	
	
$(BDIR)/simple_gen.o : simple_gen.c simple_gen.h node.h eval.h 
	$(DCC) $(CFLAGS) -c -o $@ $<
	
	
$(BDIR)/x86_gen.o : x86_gen.c node.h eval.h
	$(DCC) $(CFLAGS) -c -o $@ $<

	
	
$(BDIR)/main.o : main.c lexer.h tokens.h
	$(DCC) $(CFLAGS) -c -o $@ $<

cc : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	
dcc : dcc.c
	$(DCC) $(CFLAGS) -o $@ $<
	
clean :
	rm -f $(OBJ)
