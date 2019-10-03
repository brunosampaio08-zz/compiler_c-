%{
/*
 *Alunos:
 *Bruno Sampaio Leite   120213
 *Talita Ludmila de Lima    120895
*/

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

int num_linhas=1;
%}

DIGITO [0-9]
LETRA [a-zA-Z]
OUTRO [^0-9a-zA-Z";""-""/""=""\""+""*""("")""{""}""\[""\]"",""\n""\t""<""<="">"">=""==""!="" "]

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
                if(c=='\n') num_linhas++;
            }while(!(d == '*' && c == '/'));
        }
    }
"else"          return ELSE;
"if"            return IF;
"int"           return INT;
"return"        return RETURN;
"void"          return VOID;
"while"         return WHILE;
{DIGITO}+		return NUM;
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
<<EOF>>         return FIM;
[\n]            {num_linhas++; return ENT;}
[\t]            return TAB;
[ ]+
{OUTRO}+		return ERR;

%%

void exp(){
    termo();
    while((tok=='+')||(tok=='-')){
        eat(tok);
        termo();
    }
}

void termo(){
    fator();
    while((tok=='*')||(tok=='/')){
        eat(tok);
        fator();
    }
}

void fator(){
    switch(tok){
        case ( :
            eat(();
            exp();
            eat());
        case NUM :
            eat(NUM);
        default:
            ERRO();
    }
}

int main(int argc, char *argv[]){
/*
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
        if(token==ERR)
            printf("ERRO na linha %d, correspondente ao lexema '%s' \n", num_linhas, yytext);
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
            case NUM: fprintf(outputFile,"NUM ");       break;
            case ID:  fprintf(outputFile,"ID ");        break;
            case SOM: fprintf(outputFile,"SOM ");       break;
            case SUB: fprintf(outputFile,"SUB ");       break;
            case MUL: fprintf(outputFile,"MUL ");       break;
            case DIV: fprintf(outputFile,"DIV ");       break;
            case MEN: fprintf(outputFile," MEN ");      break;
            case IME: fprintf(outputFile," IME ");      break;
            case MAI: fprintf(outputFile," MAI ");      break;
            case IMA: fprintf(outputFile," IMA ");      break;
            case IGL: fprintf(outputFile," IGL ");      break;
            case DIF: fprintf(outputFile," DIF ");      break;
            case ATR: fprintf(outputFile," ATR ");      break;
            case PEV: fprintf(outputFile,"PEV ");       break;
            case VIR: fprintf(outputFile,"VIR ");       break;
            case APR: fprintf(outputFile,"APR ");       break;
            case FPR: fprintf(outputFile,"FPR ");       break;
            case ACL: fprintf(outputFile,"ACL ");       break;
            case FCL: fprintf(outputFile,"FCL ");       break;
            case ACH: fprintf(outputFile,"ACH ");       break;
            case FCH: fprintf(outputFile,"FCH ");       break;
            case ENT: fprintf(outputFile,"\n ");        break;
            case TAB: fprintf(outputFile,"\t");         break;
            case ELSE: fprintf(outputFile,"ELSE ");     break;
            case IF: fprintf(outputFile,"IF ");         break;
            case INT: fprintf(outputFile,"INT ");       break;
            case RETURN: fprintf(outputFile,"RETURN "); break;
            case VOID: fprintf(outputFile,"VOID ");     break;
            case WHILE: fprintf(outputFile,"WHILE ");   break;
            case ERR: fprintf(outputFile,"ERR ");       break;
        }
    i++;
    }

    //END OF SWITCH CASE WHILE

    free(fileBuffer);
    fclose(inputFile);
    fclose(outputFile);
*/
    advance();
    exp();

    return (0);
}