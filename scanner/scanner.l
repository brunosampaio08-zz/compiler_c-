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

    FILE *inputFile, *outputFile;

    fileBuffer = malloc(sizeof(int));
    fileBuffer[0] = 0;

    if(argc == 2){
        if(inputFile = fopen(argv[1], "r")){
            yyin = inputFile;
        }
        else{
            yyin = stdin;
        }
    }

    outputFile = fopen("saida.txt","w");
    i = 1;
    while((token = yylex()) != FIM){
        i++;
        fileBuffer = realloc(fileBuffer, i*sizeof(int));
        fileBuffer[i-2] = token;
        fileBuffer[i-1] = 0;
    }

    //SWITCH CASE WHILE

    i = 0;

    while (fileBuffer[i] != 0){
        switch(fileBuffer[i])
        {
            case NUM: fprintf(outputFile,"NUM "); break;
            case ID:  fprintf(outputFile,"ID ");  break;
            case SOM: fprintf(outputFile,"+ ");   break;
            case SUB: fprintf(outputFile,"- ");   break;
            case MUL: fprintf(outputFile,"* ");   break;
            case DIV: fprintf(outputFile,"/ ");   break;
            case MEN: fprintf(outputFile," < ");  break;
            case IME: fprintf(outputFile," <= "); break;
            case MAI: fprintf(outputFile," > ");  break;
            case IMA: fprintf(outputFile," >= "); break;
            case IGL: fprintf(outputFile," == "); break;
            case DIF: fprintf(outputFile," != "); break;
            case ATR: fprintf(outputFile," = ");  break;
            case PEV: fprintf(outputFile,"; ");   break;
            case VIR: fprintf(outputFile,", ");   break;
            case APR: fprintf(outputFile,"( ");   break;
            case FPR: fprintf(outputFile,") ");   break;
            case ACL: fprintf(outputFile,"[ ");   break;
            case FCL: fprintf(outputFile,"] ");   break;
            case ACH: fprintf(outputFile,"{ ");   break;
            case FCH: fprintf(outputFile,"} ");   break;
            case ENT: fprintf(outputFile,"\n ");  break;
            case TAB: fprintf(outputFile,"\t");   break;
            case ERR: fprintf(outputFile,"ERR "); break;
            case ELSE: fprintf(outputFile,"else ");   break;
            case IF: fprintf(outputFile,"if ");   break;
            case INT: fprintf(outputFile,"int ");   break;
            case RETURN: fprintf(outputFile,"return ");  break;
            case VOID: fprintf(outputFile,"void "); break;
            case WHILE: fprintf(outputFile,"while "); break;
        }
    i++;
    }

    //END OF SWITCH CASE WHILE

    free(fileBuffer);
    fclose(inputFile);
    fclose(outputFile);

    return (0);
}