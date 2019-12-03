%{
/*
 *Alunos:
 *Bruno Sampaio Leite   120213
 *Talita Ludmila de Lima    120895
*/

#include "parser.tab.h"
#include "globals.h"
#include "util.h"
#include "scan.h"

int yywrap(void){
  return 1;
}

char tokenString[MAXTOKENLEN+1];

%}

DIGITO [0-9]
LETRA [a-zA-Z]

%%

"/*"    {   
        char c;
        char d;
        c = input();
        if(c!=EOF)
        {
            do
            {
                d=c;
                c = input();
                if(c==EOF) break;
                if(c=='\n') lineno++;
            }while(!(d == '*' && c == '/'));
        }
    }
"else"          return ELSE;
"if"            return IF;
"int"           return INT;
"return"        return RETURN;
"void"          return VOID;
"while"         return WHILE;
{DIGITO}+		    return NUM;
{LETRA}({LETRA})*	return ID;
"+"		    	return SOM;
"-" 	    	return SUB;
"*"				return MUL;
"/"				return DIV;
"<"             return MEN;
"<="            return IME; 
">"             return MAI;
">="            return IMA;
"=="            return IGL;
"!="            return DIF;
"="             return ATR;
";"				return PEV;
","             return VIR;
"("				return APR;
")"				return FPR;
"["             return ACL;
"]"             return FCL;
"{"             return ACH;
"}"             return FCH;
[\n]            {lineno++;}
[ \t]+
.		return ERR;

%%

TokenType getToken(void)
{ 
  static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { 
    firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) 
  {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}