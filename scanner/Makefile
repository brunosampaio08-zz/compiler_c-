# Makefile for Cbassier compiler

CC = gcc
BISON = bison
LEX = flex

BIN = compiler

OBJS = parser.tab.o lex.yy.o main.o util.o symtable.o analyze.o

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN)

main.o: main.c globals.h util.h scan.h analyze.h
	$(CC) -c main.c

util.o: util.c util.h globals.h
	$(CC) -c util.c

symtable.o: symtable.c symtable.h
	$(CC) -c symtable.c

analyze.o: analyze.c globals.h symtable.h analyze.h
	$(CC) -c analyze.c

lex.yy.o: cbassier.lex scan.h util.h globals.h
	$(LEX) -o lex.yy.c scanner.l
	$(CC) -c lex.yy.c

cbassier.tab.o: cbassier.y globals.h
	$(BISON) -d parser.y
	$(CC) -c parser.tab.c

clean:
	-rm -f $(BIN)
	-rm -f parser.tab.c
	-rm -f parser.tab.h
	-rm -f lex.yy.c
	-rm -f $(OBJS)
