# Makefile for C- compiler

CC = gcc
BISON = bison
LEX = flex

BIN = compiler

OBJS = parser.tab.o lex.yy.o main.o util.o symboltable.o analyze.o

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN)

main.o: main.c globals.h util.h scan.h analyze.h
	$(CC) -c main.c

util.o: util.c util.h globals.h
	$(CC) -c util.c

symtable.o: symboltable.c symboltable.h
	$(CC) -c symboltable.c

analyze.o: analyze.c globals.h symboltable.h analyze.h
	$(CC) -c analyze.c

lex.yy.o: scanner.l scan.h util.h globals.h
	$(LEX) -o lex.yy.c scanner.l
	$(CC) -c lex.yy.c

parser.tab.o: parser.y globals.h
	$(BISON) -d parser.y
	$(CC) -c parser.tab.c

clean:
	-rm -f $(BIN)
	-rm -f parser.tab.c
	-rm -f parser.tab.h
	-rm -f lex.yy.c
	-rm -f $(OBJS)
