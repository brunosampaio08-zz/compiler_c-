%{

#include <stdlib.h>
#include <stdio.h>

#define NUM 300
#define ID  301
#define SOM 302
#define SUB 303
#define MUL 304
#define DIV 305
#define MEN 306 
#define IME 307
#define MAI 308
#define IMA 309
#define IGL 310
#define DIF 311
#define ATR 312
#define PEV 313
#define VIR 314
#define APR 315
#define FPR 316
#define ACL 317
#define FCL 318
#define ACH 319
#define FCH 320 
#define ACM 321
#define FCM 322
#define FIM 323
#define ERR 324
#define ENT 325
#define TAB 326
#define ELSE 327
#define IF 328
#define INT 329
#define RETURN 330
#define VOID 331
#define WHILE 332
#define COM 333

%}

DIGITO [0-9]
LETRA [a-zA-Z]
OUTRO [^0-9a-zA-Z";""-""/""=""\""+""*""("")""{""}""\[""\]"",""\n""\t""<""<="">"">=""==""!="" ""/*""*/"]

%%

"else"          return ELSE;
"if"            return IF;
"int"           return INT;
"return"        return RETURN;
"void"          return VOID;
"while"         return WHILE;
"/*"[^(/\*)(\*/)]*"*/" return COM;
{DIGITO}+		return NUM;
{LETRA}({LETRA}|{DIGITO})*	return ID;
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
"/*"            return ACM;
"*/"            return FCM;
<<EOF>>         return FIM;
[\n]            return ENT;
[\t]            return TAB;
[ ]+
{OUTRO}+		return ERR;

%%

int main(int argc, char *argv[]){
    int token, i;
    int *fileBuffer;

    FILE *inputFile;

    fileBuffer = malloc(sizeof(int));
    fileBuffer[0] = 0;

    if(arg == 2){
        if(inputFile = fopen(argv[1], "r")){
            yyin = inputFile;
        }
        else{
            yyin = stdin;
        }
    }

    i = 1;
    while(token = yylex() != FIM){
        i++;
        fileBuffer = realloc(fileBuffer, i*sizeof(int));
        fileBuffer[i-2] = token;
        fileBuffer[i-1] = 0;
    }

    //SWITCH CASE WHILE

    i = 0;
    while (buf[i] != 0)
    {
        switch(buf[i])
        {
            case NUM: printf("NUM "); break;
            case ID:  printf("ID ");  break;
            case SOM: printf("+ ");   break;
            case SUB: printf("- ");   break;
            case MUL: printf("* ");   break;
            case DIV: printf("/ ");   break;
            case MEN: printf(" < ");  break;
            case IME: printf(" <= "); break;
            case MAI: printf(" > ");  break;
            case IMA: printf(" >= "); break;
            case IGL: printf(" == "); break;
            case DIF: printf(" != "); break;
            case ATR: printf(" = ");  break;
            case PEV: printf("; ");   break;
            case VIR: printf(", ");   break;
            case APR: printf("( ");   break;
            case FPR: printf(") ");   break;
            case ACL: printf("[ ");   break;
            case FCL: printf("] ");   break;
            case ACH: printf("{ ");   break;
            case FCH: printf("} ");   break;
            case ENT: printf("\n ");  break;
            case TAB: printf("\t");   break;
            case ERR: printf("ERR "); break;
            case ELSE: printf("else ");   break;
            case IF: printf("if ");   break;
            case INT: printf("int ");   break;
            case RETURN: printf("return ");  break;
            case VOID: printf("void "); break;
            case WHILE: printf("while "); break;
        }
    i++;
    }

    //END OF SWITCH CASE WHILE

    free(fileBuffer);
    fclose(inputFile);

    return (0);
}