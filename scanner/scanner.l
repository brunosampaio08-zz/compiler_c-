%{


%}

%%

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

    //END OF SWITCH CASE WHILE

    free(fileBuffer);
    fclose(inputFile);

    return 0;
}