/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

#define SIZE 211

static int hash ( char * key );

typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

typedef struct BucketListRec {
  char * name;
  TreeNode * treeNode;
  LineList lines;
  int memloc; /* memory location for variable */
  struct BucketListRec * next;
  ExpType type;
} * BucketList;

/*
 *Scope definitions
*/

//Scope List
typedef struct ScopeListRec 
{
  char * name; // function name
  int nestedLevel;
  struct ScopeListRec *parent;
  BucketList hashTable[SIZE]; /* the hash table */
  ExpType type;
  int scopeCount;
} * Scope;

// global scope to cover function definitions
Scope globalScope;

//Scope List to output
static Scope scopeList[SIZE];
static int sizeOfList = 0;

//Stack to deal with scope
static Scope scopeStack[SIZE];
static int topScope = 0;

Scope newScope(char * scopeName, ExpType type);
void popScope(void);
void pushScope(Scope scope);
void insertScopeToList(Scope scope);
Scope currScope();
Scope st_lookup_scope(char * scopeName);
BucketList st_lookup_all_scope(char * name);
void insertLines(char* name, int lineno);
int st_lookup_mempos(char *varName, char *scopeName);
BucketList st_lookup(char * name);
void st_changeFuncLine(char *scopeName, int newLine, int isEnd);
int st_returnFuncLine(char *scopeName, int isEnd);
int st_isGlobal(char* varName, char* scopeName);

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert(char * scopeName, char * name, ExpType type, TreeNode * streeNode, int loc);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */

  void printSymTab(FILE * listing);

#endif
