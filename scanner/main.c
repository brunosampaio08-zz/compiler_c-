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
#define NO_CODE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;
FILE * errorfile;

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
  globalScope = (Scope) malloc(sizeof(Scope));
  globalScope = newScope("global");
  char pgm[120]; /* source code file name */
  if (argc != 2)
    { fprintf(stderr,"usage: %s <filename>\n",argv[0]);
      exit(1);
    }
  strcpy(pgm,argv[1]) ;
  if (strchr (pgm, '.') == NULL)
     strcat(pgm,".cm");
  source = fopen(pgm,"r");
  if (source==NULL)
  { fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  errorfile = fopen("errorfile.txt", "w");
  if(errorfile == NULL){
    fprintf(stderr, "Could not open file to print errors.\n");
  }
  fprintf(errorfile, "\\----------------------------------\\\n");
  fprintf(errorfile, "ERROS SINTATICOS DO ARQUIVO %s:\n\n", pgm);
  listing = fopen("output.txt", "w"); //Print syntax tree and symbol table to file output.txt
  fprintf(listing,"\nCompilação C-: %s\n",pgm);
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
    //if(st_lookup_scope("main") != NULL){
      if (TraceAnalyze) fprintf(listing,"\nChecando tipos...\n");
      typeCheck(syntaxTree);
      if (TraceAnalyze) fprintf(listing,"\nChecagem de tipos concluida\n");
    //}
  //}
#if !NO_CODE
  if (! Error)
  { char * codefile;
    int fnlen = strcspn(pgm,".");
    strncpy(codefile,pgm,fnlen);
    codefile = (char *) calloc(fnlen+4, sizeof(char));
    strcat(codefile,".tm");
    code = fopen(codefile,"w");
    if (code == NULL)
    { printf("Unable to open %s\n",codefile);
      exit(1);
    }
    codeGen(syntaxTree,codefile);
    fclose(code);
  }
#endif
#endif
#endif
  fclose(source);
  return 0;
}

