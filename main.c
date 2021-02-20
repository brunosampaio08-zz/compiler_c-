/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/


#include "globals.h"
#include "symboltable.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "codegenerate.h"
#include "assemblygenerate.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * icodefile;
FILE * errorfile;
FILE * assemblycode;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = FALSE;

int Error = FALSE;

Scope globalScope;

int main( int argc, char * argv[] )
{ TreeNode * syntaxTree;
  QuadrupleList QList;
  
  globalScope = (Scope) malloc(sizeof(Scope));
  globalScope = newScope("global", Void);
  
  QList = malloc(sizeof(struct QuadrupleListT));
  QList->CurrQuad.Lab = -1;
  QList->CurrQuad.QuadT.QType = -1;
  QList->next = NULL;

  char pgm[120]; /* source code file name */
  if (argc != 2)
    { fprintf(stderr,"usage: %s <filename>\n",argv[0]);
      exit(1);
    }
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  char aux[120];
  strcpy(aux, pgm);
  strcpy(pgm, "input/");
  strcat(pgm, aux);
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  
  errorfile = fopen("output/errorfile.txt", "w");
  if(errorfile == NULL){
    fprintf(stderr, "Could not open file to print errors.\n");
  }
  fprintf(errorfile, "\\----------------------------------\\\n");
  fprintf(errorfile, "ERROS SINTATICOS DO ARQUIVO %s:\n\n", pgm);
  
  listing = fopen("output/output.txt", "w"); //Print syntax tree and symbol table to file output.txt
  if(listing == NULL){
    fprintf(stderr, "Could not open output file to print.\n");
  }
  fprintf(listing,"\nCompilação C-: %s\n",pgm);

  icodefile = fopen("output/icodefile.txt", "w");
  if(icodefile == NULL){
    fprintf(stderr, "Could not open intermediate code file");
  }

  assemblycode = fopen("output/assemblycode.txt", "w");
  if(assemblycode == NULL){
    fprintf(stderr, "Could not open assembly code file");
  }

#if NO_PARSE
  while (getToken()!=ENDFILE);
#else
  syntaxTree = parse();
  if (TraceParse) {
    fprintf(listing,"\nArvore Sintatica:\n");
    printTree(syntaxTree);
  }
#if !NO_ANALYZE
  //if (! Error)
  //{ 
    fprintf(errorfile, "\n\\---------------------------------\\\n");
    fprintf(errorfile, "ERROS SEMANTICOS DO ARQUIVO %s:\n\n", pgm);
    if (TraceAnalyze) fprintf(listing,"\nConstruindo a tabela de simbolos...\n");
    buildSymtab(syntaxTree);
  //}
#if !NO_CODE
  if (! Error)
  { 
    fprintf(icodefile, "\n\\----------------------------------\\\n");
    fprintf(icodefile, "CODIGO INTERMEDIARO DO ARQUIVO %s:\n\n", pgm);
    printCode(QList, syntaxTree);
    printQList(QList);
  }
#endif
#endif
#endif
  fclose(source);
  return 0;
}

