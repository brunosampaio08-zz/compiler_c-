#include <math.h>
#include "globals.h"
#include "codegenerate.h"
#include "symboltable.h"
#include "assemblygenerate.h"

//Code goes from pos 1 to pos 5000
#define codeStart 1
//Temporarys go from pos 5001 to pos 5033
#define tempStart 5001
//Heap starts at 5034 and goes up
#define heapStart 5034
//Global variables start at 9999 and ends at nextFramePointer (9500)
#define globalStart 10000
//Stack mem pos start (1 before pointers area end)
#define stackStart 9497 
//Last mem pos is next frame pointer
#define nextFramePointer 9499
//Current frame pointer is position 9498
#define currFramePointer 9498
//Heap pointer is mem position 9497
#define heapPointer 9497

/* Global variables definition */
FILE * assemblycode;
FILE * binarycode;
int codeLine = codeStart;

/* Scope name stack definitions */
char *scopeNameStack[SIZE];
int scopeNameTop = 0;

void pushScopeNameStack(char *name){
    scopeNameStack[scopeNameTop++] = name;
}

void popScopeNameStack(){
    scopeNameTop--;
}

char *currScopeName(){
    return scopeNameStack[scopeNameTop-1];
}

/* Label array definitions and functions */
int currentLabel = 0;
int topLabel = 255;
int *labelArray;

void insertLabel(int line_num, int label_num){
    if(currentLabel == 0){
        labelArray = malloc(topLabel*sizeof(int));
    }else{
        if(currentLabel == topLabel-1){
            topLabel = topLabel*2;
            labelArray = realloc(labelArray, topLabel*sizeof(int));
        }
    }

    labelArray[label_num] = line_num;
    currentLabel++;
}

int getLabel(int label_num){
    return labelArray[label_num];
}


void getAddress(assemblyListS A, QuadrupleList Q, varFieldType vfType, int field){
    switch (vfType)
    {
        case tempVar:
            //Temporarios sempre têm endereco tempStart (base) + offset (predefinido em software)
            switch (field)
            {
            case 1:
                fprintf(assemblycode, "LDXI %d\n", tempStart+Q->CurrQuad.firstField.val);
                insertAssemblyLine(A, LDXI, number, NULL, tempStart+Q->CurrQuad.firstField.val);
                
                break;
            case 2:
                fprintf(assemblycode, "LDXI %d\n", tempStart+Q->CurrQuad.secField.val);
                insertAssemblyLine(A, LDXI, number, NULL, tempStart+Q->CurrQuad.secField.val);
                
                break;

            case 3:
                fprintf(assemblycode, "LDXI %d\n", tempStart+Q->CurrQuad.thirdField.val);
                insertAssemblyLine(A, LDXI, number, NULL, tempStart+Q->CurrQuad.thirdField.val);
                
                break;
            default:
                break;
            }

            break;

        case Var:
            switch (field)
            {
            case 1:
                if (!st_isGlobal(Q->CurrQuad.firstField.name, currScopeName()))
                {
                    fprintf(assemblycode, "LDXI %d\n", 
                        currFramePointer); //Pega o apontador de current frame pointer        
                    insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);
                    
                    fprintf(assemblycode, "LDA 0\n"); //Pega o endereço do registro atual
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                    
                    fprintf(assemblycode, "TAX\n"); //X recebe endereço base do reg. de ativ.
                    insertAssemblyLine(A, TAX, -1, NULL, -1);
                    
                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);
                
                }else{
                    fprintf(assemblycode, "LDXI %d\n", 
                        globalStart); //Pega o apontador para a area de alocacoes globais
                    insertAssemblyLine(A, LDXI, number, NULL, globalStart);
                
                }
                
                break;
            case 2:
                if (!st_isGlobal(Q->CurrQuad.secField.name, currScopeName()))
                {
                    fprintf(assemblycode, "LDXI %d\n", 
                        currFramePointer); //Pega o apontador de current frame pointer
                     insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);
                     
                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);
                    
                    fprintf(assemblycode, "LDA 0\n"); //Pega o endereço do registro atual
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                    
                    fprintf(assemblycode, "TAX\n"); //X recebe endereço base do reg. de ativ.
                    insertAssemblyLine(A, TAX, -1, NULL, -1);
                    
                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);     
                
                }else{
                    fprintf(assemblycode, "LDXI %d\n", 
                        globalStart); //Pega o apontador para a area de alocacoes globais
                    insertAssemblyLine(A, LDXI, number, NULL, globalStart);
                
                }

                break;
            case 3:
                if (!st_isGlobal(Q->CurrQuad.thirdField.name, currScopeName()))
                {
                    fprintf(assemblycode, "LDXI %d\n", 
                        currFramePointer); //Pega o apontador de current frame pointer   
                    insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);      

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);
                    
                    fprintf(assemblycode, "LDA 0\n"); //Pega o endereço do registro atual
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                    
                    fprintf(assemblycode, "TAX\n"); //X recebe endereço base do reg. de ativ.
                    insertAssemblyLine(A, TAX, -1, NULL, -1);
                    
                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1); 
                
                }else{
                    fprintf(assemblycode, "LDXI %d\n", 
                        globalStart); //Pega o apontador para a area de alocacoes globais
                    insertAssemblyLine(A, LDXI, number, NULL, globalStart);
                
                }

                break;
            default:
                break;
            }
            

            break;

        case arrVar:
            switch (field)
            {
            case 1:
                if (!st_isGlobal(Q->CurrQuad.firstField.arr.name, currScopeName()))
                {
                    fprintf(assemblycode, "LDXI %d\n", 
                        currFramePointer); //Pega o apontador de current frame pointer        
                    insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1); 
                    
                    fprintf(assemblycode, "LDA 0\n"); //Pega endereço base do registro atual
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                    
                
                }else{
                    fprintf(assemblycode, "LDXI %d\n", 
                        globalStart); //Pega o apontador para a area de alocacoes globais
                    insertAssemblyLine(A, LDXI, number, NULL, globalStart);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);  

                    //Acc recebe base da area global
                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);
                
                }
            
            //Soma endereço base do reg. de ativ. com offset da sym_tab = contem endereço base do vet na mem
                fprintf(assemblycode, "SBCI %d\n", 
                    st_lookup_mempos(Q->CurrQuad.firstField.arr.name, currScopeName()));
                insertAssemblyLine(A, SBCI, number, NULL, 
                    st_lookup_mempos(Q->CurrQuad.firstField.arr.name, currScopeName()));
                
                fprintf(assemblycode, "TAX\n"); //X = endereco q contem endereço base do vet
                insertAssemblyLine(A, TAX, -1, NULL, -1);
                
                fprintf(assemblycode, "LDA 0\n"); //Acc contem endereço base do vet no heap
                insertAssemblyLine(A, LDA, number, NULL, 0);
                
                //X recebe endereco do temporario que contem o calc do indice
                fprintf(assemblycode, "LDXI %d\n", Q->CurrQuad.firstField.arr.arrNum+tempStart);
                insertAssemblyLine(A, LDXI, number, NULL, Q->CurrQuad.firstField.arr.arrNum+tempStart);
                
                fprintf(assemblycode, "SBC 0\n"); //Acc = endereco base do vet no heap + mem[temporario] (indice)
                insertAssemblyLine(A, ADC, number, NULL, 0);
                
                fprintf(assemblycode, "TAX\n"); //X = endereco de interesse (vet[t0])
                insertAssemblyLine(A, TAX, -1, NULL, -1);
                
                fprintf(assemblycode, "PLA\n"); 
                insertAssemblyLine(A, PLA, -1, NULL, -1);

                break;
            case 2:
                if (!st_isGlobal(Q->CurrQuad.secField.arr.name, currScopeName()))
                {
                    fprintf(assemblycode, "LDXI %d\n", 
                        currFramePointer); //Pega o apontador de current frame pointer        
                    insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1); 
                    
                    fprintf(assemblycode, "LDA 0\n"); //Pega endereço base do registro atual
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                    
                
                }else{
                    fprintf(assemblycode, "LDXI %d\n", 
                        globalStart); //Pega o apontador para a area de alocacoes globais
                    insertAssemblyLine(A, LDXI, number, NULL, globalStart);  

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);  

                    //Acc recebe base da area global
                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);
                
                }

            //Soma endereço base do reg. de ativ. com offset da sym_tab = contem endereço base do vet na mem
                fprintf(assemblycode, "SBCI %d\n", 
                    st_lookup_mempos(Q->CurrQuad.secField.arr.name, currScopeName()));
                insertAssemblyLine(A, SBCI, number, NULL, 
                    st_lookup_mempos(Q->CurrQuad.secField.arr.name, currScopeName()));
                
                fprintf(assemblycode, "TAX\n"); //X = endereco q contem endereço base do vet
                insertAssemblyLine(A, TAX, -1, NULL, -1);
                
                fprintf(assemblycode, "LDA 0\n"); //Acc contem endereço base do vet no heap
                insertAssemblyLine(A, LDA, number, NULL, 0);
                
                //X recebe endereco do temporario que contem o calc do indice
                fprintf(assemblycode, "LDXI %d\n", Q->CurrQuad.secField.arr.arrNum+tempStart);
                insertAssemblyLine(A, LDXI, number, NULL, Q->CurrQuad.secField.arr.arrNum+tempStart);
                
                fprintf(assemblycode, "SBC 0\n"); //Acc = endereco base do vet no heap + mem[temporario]
                insertAssemblyLine(A, ADC, number, NULL, 0);
                
                fprintf(assemblycode, "TAX\n"); //X = endereco de interesse (vet[t0])
                insertAssemblyLine(A, TAX, -1, NULL, -1);
                
                fprintf(assemblycode, "PLA\n");
                insertAssemblyLine(A, PLA, -1, NULL, -1);
                
                break;

            case 3:
                if (!st_isGlobal(Q->CurrQuad.thirdField.arr.name, currScopeName()))
                {
                    fprintf(assemblycode, "LDXI %d\n", 
                        currFramePointer); //Pega o apontador de current frame pointer        
                    insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1); 
                    
                    //Acc recebe endereco base do registro
                    fprintf(assemblycode, "LDA 0\n");
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                
                }else{
                    fprintf(assemblycode, "LDXI %d\n", 
                        globalStart); //Pega o apontador para a area de alocacoes globais
                    insertAssemblyLine(A, LDXI, number, NULL, globalStart);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);  

                    //Acc recebe base da area global
                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);
                
                }
                
                fprintf(assemblycode, "SBCI %d\n", 
                    st_lookup_mempos(Q->CurrQuad.thirdField.arr.name, currScopeName()));
                insertAssemblyLine(A, SBCI, number, NULL, 
                    st_lookup_mempos(Q->CurrQuad.thirdField.arr.name, currScopeName()));
                
                fprintf(assemblycode, "TAX\n"); //X = endereco q contem endereço base do vet
                insertAssemblyLine(A, TAX, -1, NULL, -1);
                
                fprintf(assemblycode, "LDA 0\n"); //Acc contem endereço base do vet no heap
                insertAssemblyLine(A, LDA, number, NULL, 0);
                
                //X recebe endereco do temporario que contem o calc do indice
                fprintf(assemblycode, "LDXI %d\n", Q->CurrQuad.thirdField.arr.arrNum+tempStart);
                insertAssemblyLine(A, LDXI, number, NULL, Q->CurrQuad.thirdField.arr.arrNum+tempStart);
                
                fprintf(assemblycode, "SBC 0\n"); //Acc = endereco base do vet no heap + mem[temporario]
                insertAssemblyLine(A, ADC, number, NULL, 0);
                
                fprintf(assemblycode, "TAX\n"); //X = endereco de interesse (vet[t0])
                insertAssemblyLine(A, TAX, -1, NULL, -1);
                
                fprintf(assemblycode, "PLA\n");
                insertAssemblyLine(A, PLA, -1, NULL, -1);

                break;
            default:
                break;
            }

            

            break;
    }
}

void printQList(QuadrupleList QList){
    QuadrupleList Q;
    assemblyListS A;
    //If there is no scope, insert global scope
    if(scopeNameTop == 0){
        pushScopeNameStack("global");
        //Start pointers
        fprintf(assemblycode, "LDXI %d\n", nextFramePointer);
        A = createAssemblyList(LDXI, number, NULL, nextFramePointer);

        fprintf(assemblycode, "LDAI %d\n", stackStart);
        insertAssemblyLine(A, LDAI, number, NULL, stackStart);
        
        fprintf(assemblycode, "STA 0\n");
        insertAssemblyLine(A, STA, number, NULL, 0);
        
        fprintf(assemblycode, "LDXI %d\n", currFramePointer);
        insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);
        
        fprintf(assemblycode, "STA 0\n"); //next FP e curr FP apontam pra base da pilha
        insertAssemblyLine(A, STA, number, NULL, 0);
        
        fprintf(assemblycode, "TAX\n");
        insertAssemblyLine(A, TAX, -1, NULL, -1);
        
        fprintf(assemblycode, "TXS\n"); //Coloca apontador da pilha na posicao correta
        insertAssemblyLine(A, TXS, -1, NULL, -1);
        
        fprintf(assemblycode, "LDXI %d\n", heapPointer);
        insertAssemblyLine(A, LDXI, number, NULL, heapPointer);
        
        fprintf(assemblycode, "LDAI %d\n", heapStart);
        insertAssemblyLine(A, LDAI, number, NULL, heapStart);
        
        fprintf(assemblycode, "STA 0\n"); //HeapPointer aponta pro comeco do heap
        insertAssemblyLine(A, STA, number, NULL, 0);
    }
    for(Q = QList; Q != NULL; Q = Q->next){
        switch (Q->CurrQuad.Lab)
        {
        case CalcQuad:
            //Primeiro pega firstField (estara em acc)
            //Depois pega secField (sera operado da memoria)
            //Depois pega thirdField (endereco do store)

            switch (Q->CurrQuad.ffType)
            {
                case Var:
                    getAddress(A, Q, Var, 1);
                    //Faz load na posicao base (X) + offset (sym_tab)
                    fprintf(assemblycode, "LDA %d\n",
                        -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName())); 
                    insertAssemblyLine(A, LDA, number, NULL, 
                        -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName())); 
                    

                break;
                
                case tempVar:
                    getAddress(A, Q, tempVar, 1);
                    //Faz load da pos X
                    fprintf(assemblycode, "LDA 0\n");
                    insertAssemblyLine(A, LDA, number, NULL, 0);

                    break;

                case arrVar:
                    getAddress(A, Q, arrVar, 1);
                    //Faz load da pos X
                    fprintf(assemblycode, "LDA 0\n");
                    insertAssemblyLine(A, LDA, number, NULL, 0);

                    break;
                
                case constVar:
                    fprintf(assemblycode, "LDAI %d\n", Q->CurrQuad.firstField.val);
                    insertAssemblyLine(A, LDAI, number, NULL, Q->CurrQuad.firstField.val);
                    

                    break;

                default:
                    break;
            }

            switch (Q->CurrQuad.sfType)
            {
                case Var:
                    getAddress(A, Q, Var, 2);
                    switch (Q->CurrQuad.QuadT.TType)
                    {
                        case SOM:
                            fprintf(assemblycode, "ADC %d\n", 
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            insertAssemblyLine(A, ADC, number, NULL,
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            
                        
                        break;

                        case SUB:
                            fprintf(assemblycode, "SBC %d\n", 
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            insertAssemblyLine(A, SBC, number, NULL,
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            

                        break;

                        case IME:
                        case MEN:
                        case MAI:
                        case IMA:
                        case IGL:
                        case DIF:
                            fprintf(assemblycode, "CMP %d\n", 
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            insertAssemblyLine(A, CMP, number, NULL,
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            

                        break;

                        case MUL:
                            fprintf(assemblycode, "MUL %d\n", 
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            insertAssemblyLine(A, MULP, number, NULL,
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            
                            
                        break;

                        case DIV:
                            fprintf(assemblycode, "DIV %d\n", 
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            insertAssemblyLine(A, DIVS, number, NULL,
                                -st_lookup_mempos(Q->CurrQuad.secField.name, currScopeName()));
                            

                        break;

                    }
                
                break;

                case tempVar:
                    getAddress(A, Q, tempVar, 2);
                    switch (Q->CurrQuad.QuadT.TType)
                    {
                        case SOM:
                            fprintf(assemblycode, "ADC 0\n");
                            insertAssemblyLine(A, ADC, number, NULL, 0);
                            

                        break;

                        case SUB:
                            fprintf(assemblycode, "SBC 0\n");
                            insertAssemblyLine(A, SBC, number, NULL, 0);
                            

                        break;

                        case IME:
                        case MEN:
                        case MAI:
                        case IMA:
                        case IGL:
                        case DIF:
                            fprintf(assemblycode, "CMP 0\n");
                            insertAssemblyLine(A, CMP, number, NULL, 0);
                            

                        break;

                        case MUL:
                            fprintf(assemblycode, "MUL 0\n");
                            insertAssemblyLine(A, MULP, number, NULL, 0);
                            

                        break;

                        case DIV:
                            fprintf(assemblycode, "DIV 0\n");
                            insertAssemblyLine(A, DIVS, number, NULL, 0);
                            

                        break;

                    }
                break;

                case arrVar:
                    getAddress(A, Q, arrVar, 2);
                    switch (Q->CurrQuad.QuadT.TType)
                    {
                        case SOM:
                            fprintf(assemblycode, "ADC 0\n");
                            insertAssemblyLine(A, ADC, number, NULL, 0);
                            

                        break;

                        case SUB:
                            fprintf(assemblycode, "SBC 0\n");
                            insertAssemblyLine(A, SBC, number, NULL, 0);
                            
                            
                        break;

                        case IME:
                        case MEN:
                        case MAI:
                        case IMA:
                        case IGL:
                        case DIF:
                            fprintf(assemblycode, "CMP 0\n");
                            insertAssemblyLine(A, CMP, number, NULL, 0);
                            

                        break;

                        case MUL:
                            fprintf(assemblycode, "MUL 0\n");
                            insertAssemblyLine(A, MULP, number, NULL, 0);
                            

                        break;

                        case DIV:
                            fprintf(assemblycode, "DIV 0\n");
                            insertAssemblyLine(A, DIVS, number, NULL, 0);
                            

                        break;

                    }
                
                break;

                case constVar:

                    switch (Q->CurrQuad.QuadT.TType)
                    {
                        case SOM:
                            fprintf(assemblycode, "ADCI %d\n", Q->CurrQuad.secField.val);
                            insertAssemblyLine(A, ADCI, number, NULL, Q->CurrQuad.secField.val);
                            

                        break;

                        case SUB:
                            fprintf(assemblycode, "SBCI %d\n", Q->CurrQuad.secField.val);
                            insertAssemblyLine(A, SBCI, number, NULL, Q->CurrQuad.secField.val);
                            

                        break;

                        case IME:
                        case MEN:
                        case MAI:
                        case IMA:
                        case IGL:
                        case DIF:
                            fprintf(assemblycode, "CPI %d\n", Q->CurrQuad.secField.val);
                            insertAssemblyLine(A, CPI, number, NULL, Q->CurrQuad.secField.val);
                            

                        break;

                        case MUL:
                            fprintf(assemblycode, "MULI %d\n", Q->CurrQuad.secField.val);
                            insertAssemblyLine(A, MULPI, number, NULL, Q->CurrQuad.secField.val);
                            

                        break;

                        case DIV:
                            fprintf(assemblycode, "DIVI %d\n", Q->CurrQuad.secField.val);
                            insertAssemblyLine(A, DIVSI, number, NULL, Q->CurrQuad.secField.val);
                            

                        break;
                    }

                break;

            }

            switch (Q->CurrQuad.tfType)
            {
                case Var:
                    getAddress(A, Q, Var, 3);
                    //Faz load na posicao base (X) + offset (sym_tab)
                    fprintf(assemblycode, "STA %d\n", 
                        -st_lookup_mempos(Q->CurrQuad.thirdField.name, currScopeName()));
                    insertAssemblyLine(A, STA, number, NULL,
                        -st_lookup_mempos(Q->CurrQuad.thirdField.name, currScopeName()));
                    

                    break;
                
                case tempVar:
                    getAddress(A, Q, tempVar, 3);
                    //Faz load da pos X
                    fprintf(assemblycode, "STA 0\n");
                    insertAssemblyLine(A, STA, number, NULL, 0);
                    
                    break;

                case arrVar:
                    getAddress(A, Q, arrVar, 3);
                    //Faz load da pos X
                    fprintf(assemblycode, "STA 0\n");
                    insertAssemblyLine(A, STA, number, NULL, 0);
                    

                    break;
                    
                default:
                    break;
            }

        break;
        
        case NQuad:
            switch (Q->CurrQuad.QuadT.QType)
            {
            case FUN_DECL:
                fprintf(assemblycode, "     %s\n", Q->CurrQuad.thirdField.name);
                int isMain = strcmp(Q->CurrQuad.thirdField.name, "main"); 
                pushScopeNameStack(Q->CurrQuad.thirdField.name);
                st_changeFuncLine(Q->CurrQuad.thirdField.name, codeLine, 0);
                if(isMain != 0){
                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "TAY\n"); //Acc tem PC (logo apos JSR)
                    insertAssemblyLine(A, TAY, -1, NULL, -1);
                    
                }
                fprintf(assemblycode, "LDXI %d\n", nextFramePointer); //Ponteiro de next frame pointer
                insertAssemblyLine(A, LDXI, number, NULL, nextFramePointer);

                fprintf(assemblycode, "LDA 0\n"); //Acc tem endereço de next frame pointer
                insertAssemblyLine(A, LDA, number, NULL, 0);
                
                fprintf(assemblycode, "LDXI %d\n", currFramePointer); //Ponteiro de current frame pointer
                insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);
                
                fprintf(assemblycode, "STA 0\n"); //Current frame pointer recebe next frame pointer
                insertAssemblyLine(A, STA, number, NULL, 0);
                
                //Acc passa a ter next frame pointer + reserva de endereços para
                //Argumentos e locais
                fprintf(assemblycode, "SBCI %d\n", 
                    st_lookup_scope(Q->CurrQuad.thirdField.name)->scopeCount);
                insertAssemblyLine(A, SBCI, number, NULL, 
                    st_lookup_scope(Q->CurrQuad.thirdField.name)->scopeCount);
                
                fprintf(assemblycode, "TAX\n"); //Acc para X
                insertAssemblyLine(A, TAX, -1, NULL, -1);
                
                fprintf(assemblycode, "TXS\n"); //Pilha apontara para o fim da alocacao do reg de ativ.
                insertAssemblyLine(A, TXS, -1, NULL, -1);
                
                if(isMain != 0){
                    fprintf(assemblycode, "TYA\n"); //Acc recebe PC
                    insertAssemblyLine(A, TYA, -1, NULL, -1);
                
                    fprintf(assemblycode, "PHA\n"); //Coloca PC na pilha
                    insertAssemblyLine(A, PHA, -1, NULL, -1);
                }
                fprintf(assemblycode, "LDXI %d\n", currFramePointer); //Ponteiro pra current frame pointer
                insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);
                
                fprintf(assemblycode, "LDA 0\n"); //Acc recebe current frame pointer
                insertAssemblyLine(A, LDA, number, NULL, 0);
                
                fprintf(assemblycode, "PHA\n"); //Coloca current frame pointer na pilha
                insertAssemblyLine(A, PHA, -1, NULL, -1);
                
                fprintf(assemblycode, "TSX\n"); //Transfere endereco da pilha pra X
                insertAssemblyLine(A, TSX, -1, NULL, -1);
                
                fprintf(assemblycode, "TXA\n"); //Acc recebe endereco da pilha
                insertAssemblyLine(A, TXA, -1, NULL, -1);
                
                fprintf(assemblycode, "LDXI %d\n", nextFramePointer); //Ponteiro pra next frame pointer
                insertAssemblyLine(A, LDXI, number, NULL, nextFramePointer);
                
                fprintf(assemblycode, "STA 0\n"); //Next frame pointer recebe o topo da pilha
                insertAssemblyLine(A, STA, number, NULL, 0);
                
                break;

            case FUN_END:
                st_changeFuncLine(Q->CurrQuad.thirdField.name, codeLine, 1);
                if(strcmp(Q->CurrQuad.thirdField.name, "main") == 0){
                    fprintf(assemblycode, "SFF\n");
                    insertAssemblyLine(A, SFF, -1, NULL, -1);
                
                }else{
                    popScopeNameStack();

                    fprintf(assemblycode, "PLA\n"); //Pega o current frame pointer da pilha
                    insertAssemblyLine(A, PLA, -1, NULL, -1);
                    
                    fprintf(assemblycode, "LDXI %d\n", nextFramePointer); //Ponteiro pra next frame pointer
                    insertAssemblyLine(A, LDXI, number, NULL, nextFramePointer);
                    
                    fprintf(assemblycode, "STA 0\n"); //Next frame pointer recebe current frame pointer
                    insertAssemblyLine(A, STA, number, NULL, 0);
                    
                    fprintf(assemblycode, "RTS\n"); //Retorna da subrotina (desempilha PC)
                    insertAssemblyLine(A, RTS, -1, NULL, -1);
                    
                }
                
                break;

            case CALL:
                if(strcmp(Q->CurrQuad.firstField.name, "input") == 0){
                    fprintf(assemblycode, "INT\n");
                    insertAssemblyLine(A, INPT, -1, NULL, -1);

                }else if(strcmp(Q->CurrQuad.firstField.name, "output") == 0){
                    fprintf(assemblycode, "PLA\n"); //Param e 1 so e esta na pilha
                    insertAssemblyLine(A, PLA, -1, NULL, -1);
                    
                    fprintf(assemblycode, "TAY\n"); //Output sempre sai de Y
                    insertAssemblyLine(A, TAY, -1, NULL, -1);
                    
                    fprintf(assemblycode, "OUP\n");    
                    insertAssemblyLine(A, OUP, -1, NULL, -1);

                    fprintf(assemblycode, "BRK\n");
                    insertAssemblyLine(A, BRK, -1, NULL, -1);
                    
                }else if(strcmp(Q->CurrQuad.firstField.name, "main") == 0){
                    fprintf(assemblycode, "JMP main\n");
                    insertAssemblyLine(A, JMP, string, "main", -1);
                
                }else if(strcmp(Q->CurrQuad.firstField.name, "changeContext") == 0){
                    fprintf(assemblycode, "TSX\n");
                    insertAssemblyLine(A, TSX, -1, NULL, -1);

                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);

                    fprintf(assemblycode, "LDXI 0\n");
                    insertAssemblyLine(A, LDXI, number, NULL, 0);

                    fprintf(assemblycode, "STA 0\n");
                    insertAssemblyLine(A, STA, number, NULL, 0);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "ADCI 1\n");
                    insertAssemblyLine(A, ADCI, number, NULL, 1);

                    fprintf(assemblycode, "MULPI 10000\n");
                    insertAssemblyLine(A, MULPI, number, NULL, 10000);

                    fprintf(assemblycode, "TAX\n");
                    insertAssemblyLine(A, TAX, -1, NULL, -1);

                    fprintf(assemblycode, "TXAX\n");
                    insertAssemblyLine(A, TXAX, -1, NULL, -1);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "MULPI 6\n");
                    insertAssemblyLine(A, MULPI, number, NULL, 6);
                    
                    fprintf(assemblycode, "TAX\n");
                    insertAssemblyLine(A, TAX, -1, NULL, -1);

                    fprintf(assemblycode, "LDA 9504\n");
                    insertAssemblyLine(A, LDA, number, NULL, 9504);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "PLP\n");
                    insertAssemblyLine(A, PLP, -1, NULL, -1);

                    fprintf(assemblycode, "LDA 9505\n");
                    insertAssemblyLine(A, LDA, number, NULL, 9505);
                    
                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "PLPC\n");
                    insertAssemblyLine(A, PLPC, -1, NULL, -1);

                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);

                    fprintf(assemblycode, "TAY\n");
                    insertAssemblyLine(A, TAY, -1, NULL, -1);

                    fprintf(assemblycode, "LDA 9501\n");
                    insertAssemblyLine(A, LDA, number, NULL, 9501);

                    fprintf(assemblycode, "TAX\n");
                    insertAssemblyLine(A, TAX, -1, NULL, -1);

                    fprintf(assemblycode, "TXS\n");
                    insertAssemblyLine(A, TXS, -1, NULL, -1);

                    fprintf(assemblycode, "TYA\n");
                    insertAssemblyLine(A, TYA, -1, NULL, -1);

                    fprintf(assemblycode, "TAX\n");
                    insertAssemblyLine(A, TAX, -1, NULL, -1);

                    fprintf(assemblycode, "LDY 9503\n");
                    insertAssemblyLine(A, LDY, number, NULL, 9503);

                    fprintf(assemblycode, "LDA 9500\n");
                    insertAssemblyLine(A, LDA, number, NULL, 9500);

                    fprintf(assemblycode, "LDX 9502\n");
                    insertAssemblyLine(A, LDX, number, NULL, 9502);

                    fprintf(assemblycode, "SPC\n");
                    insertAssemblyLine(A, SPC, -1, NULL, -1);


                }else if(strcmp(Q->CurrQuad.firstField.name, "stackRegisters") == 0){
                   
                    fprintf(assemblycode, "OS line\n");

                    //Transfer X on Aux Reg
                    fprintf(assemblycode, "TXAX\n");
                    insertAssemblyLine(A, TXAX, -1, NULL, -1);
                    
                    
                    fprintf(assemblycode, "LDXI 0\n");
                    insertAssemblyLine(A, LDXI, number, NULL, 0);
                    
                    //Store A on temp[0] (mem[5001+0])
                    fprintf(assemblycode, "STA 5001\n");
                    insertAssemblyLine(A, STA, number, NULL, 5001);
                    
                    fprintf(assemblycode, "TSX\n");
                    insertAssemblyLine(A, TSX, -1, NULL, -1);
                    
                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);
                    
                    fprintf(assemblycode, "LDXI 0\n");
                    insertAssemblyLine(A, LDXI, number, NULL, 0);
                    
                    fprintf(assemblycode, "STA 5002\n");
                    insertAssemblyLine(A, STA, number, NULL, 5002);
                    
                    fprintf(assemblycode, "LDA 0\n");
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                    
                    fprintf(assemblycode, "TAX\n");
                    insertAssemblyLine(A, TAX, -1, NULL, -1);
                    
                    fprintf(assemblycode, "TXS\n");
                    insertAssemblyLine(A, TXS, -1, NULL, -1);
                    
                    fprintf(assemblycode, "PHPC\n");
                    insertAssemblyLine(A, PHPC, -1, NULL, -1);

                    fprintf(assemblycode, "PHP\n");
                    insertAssemblyLine(A, PHP, -1, NULL, -1);

                    fprintf(assemblycode, "TYA\n");
                    insertAssemblyLine(A, TYA, -1, NULL, -1);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "TAXX\n");
                    insertAssemblyLine(A, TAXX, -1, NULL, -1);

                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "LDXI 0\n");
                    insertAssemblyLine(A, LDXI, number, NULL, 0);

                    fprintf(assemblycode, "LDA 5002\n");
                    insertAssemblyLine(A, LDA, number, NULL, 5002);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "LDA 5001\n");
                    insertAssemblyLine(A, LDA, number, NULL, 5001);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                }else if(strcmp(Q->CurrQuad.firstField.name, "storeRegisters") == 0){
                    
                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "MULPI 6\n");
                    insertAssemblyLine(A, MULPI, number, NULL, 6);

                    fprintf(assemblycode, "TAX\n");
                    insertAssemblyLine(A, TAX, -1, NULL, -1);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "STA\n");
                    insertAssemblyLine(A, STA, number, NULL, 9500);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "STA\n");
                    insertAssemblyLine(A, STA, number, NULL, 9501);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "STA\n");
                    insertAssemblyLine(A, STA, number, NULL, 9502);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "STA\n");
                    insertAssemblyLine(A, STA, number, NULL, 9503);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "STA\n");
                    insertAssemblyLine(A, STA, number, NULL, 9504);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "STA\n");
                    insertAssemblyLine(A, STA, number, NULL, 9505);

                }else if(strcmp(Q->CurrQuad.firstField.name, "processFinished") == 0){
                    //Check Process Finish flag
                    fprintf(assemblycode, "CPF\n");
                    insertAssemblyLine(A, CPF, -1, NULL, -1);
                    
                }else if(strcmp(Q->CurrQuad.firstField.name, "setPC") == 0){
                    fprintf(assemblycode, "TSX\n");
                    insertAssemblyLine(A, TSX, -1, NULL, -1);

                    fprintf(assemblycode, "TXA\n");
                    insertAssemblyLine(A, TXA, -1, NULL, -1);

                    fprintf(assemblycode, "LDXI 0\n");
                    insertAssemblyLine(A, LDXI, number, NULL, 0);

                    fprintf(assemblycode, "STA 0\n");
                    insertAssemblyLine(A, STA, number, NULL, 0);

                    fprintf(assemblycode, "PLA\n");
                    insertAssemblyLine(A, PLA, -1, NULL, -1);

                    fprintf(assemblycode, "ADCI 1\n");
                    insertAssemblyLine(A, ADCI, number, NULL, 1);

                    fprintf(assemblycode, "MULPI 10000\n");
                    insertAssemblyLine(A, MULPI, number, NULL, 10000);

                    fprintf(assemblycode, "TAX\n");
                    insertAssemblyLine(A, TAX, -1, NULL, -1);

                    fprintf(assemblycode, "TXAX\n");
                    insertAssemblyLine(A, TXAX, -1, NULL, -1);

                    fprintf(assemblycode, "LDAI 0\n");
                    insertAssemblyLine(A, LDAI, number, NULL, 0);

                    fprintf(assemblycode, "PHA\n");
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "PLPC\n");
                    insertAssemblyLine(A, PLPC, -1, NULL, -1);

                    fprintf(assemblycode, "SPC\n");
                    insertAssemblyLine(A, SPC, -1, NULL, -1);

                }else{
                    //IDA
                    fprintf(assemblycode, "JSR %s\n", Q->CurrQuad.firstField.name); //Pula pra subrotina (Empilha PC)
                    insertAssemblyLine(A, JSR, string, Q->CurrQuad.firstField.name, -1);
                    
                    //VOLTA
                    fprintf(assemblycode, "TAX\n"); //Acc tem next frame pointer
                    insertAssemblyLine(A, TAX, -1, NULL, -1);
                    
                    fprintf(assemblycode, "TXS\n"); //Topo da pilha passa a ser next frame pointer
                    insertAssemblyLine(A, TXS, -1, NULL, -1);
                    
                    fprintf(assemblycode, "PLA\n"); //Topo da pilha tem current frame pointer antigo 
                    insertAssemblyLine(A, PLA, -1, NULL, -1);
                    
                    fprintf(assemblycode, "LDXI %d\n", currFramePointer); //Ponteiro pra current frame pointer
                    insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);
                    
                    fprintf(assemblycode, "STA 0\n"); //Atualiza current frame pointer
                    insertAssemblyLine(A, STA, number, NULL, 0);
                    
                    fprintf(assemblycode, "PHA\n"); //Empilha de volta
                    insertAssemblyLine(A, PHA, -1, NULL, -1);
                    
                }

                break;
            
            case ARR_ALLOC:
                fprintf(assemblycode, "LDXI %d\n", heapPointer); //X recebe ponteiro para heapPointer
                insertAssemblyLine(A, LDXI, number, NULL, heapPointer);
                
                fprintf(assemblycode, "LDA 0\n"); //Acc recebe valor atual de heapPointer
                insertAssemblyLine(A, LDA, number, NULL, 0);
                
                
                
                if (strcmp(currScopeName(), "global") == 0)
                {
                    //Se e um array global
                    fprintf(assemblycode, "LDXI %d\n", 
                        globalStart); //Pega o apontador para a area de alocacoes globais
                    insertAssemblyLine(A, LDXI, number, NULL, globalStart);
                
                }else{
                    //Else
                    fprintf(assemblycode, "PHA\n"); //Coloca valor do heapPointer na pilha
                    insertAssemblyLine(A, PHA, -1, NULL, -1);

                    fprintf(assemblycode, "LDXI %d\n", 
                        currFramePointer); //Pega o apontador de current frame pointer
                    insertAssemblyLine(A, LDXI, number, NULL, currFramePointer);

                    fprintf(assemblycode, "LDA 0\n"); //Acc recebe base do frame pointer atual
                    insertAssemblyLine(A, LDA, number, NULL, 0);
                    
                    fprintf(assemblycode, "TAX\n"); //X tem base do fp atual
                    insertAssemblyLine(A, TAX, -1, NULL, -1);

                    fprintf(assemblycode, "PLA\n"); //A recebe base do endereço alocado no heapPointer
                    insertAssemblyLine(A, PLA, -1, NULL, -1);
                
                }
                
                fprintf(assemblycode, "ADCI %d\n", Q->CurrQuad.secField.val); //Atualiza heapPointer
                insertAssemblyLine(A, ADCI, number, NULL, Q->CurrQuad.secField.val);
                
                //Coloca endereco base do alocado no heap em currFP - offset
                fprintf(assemblycode, "STA %d\n", 
                    -st_lookup_mempos(Q->CurrQuad.thirdField.name, currScopeName()));
                insertAssemblyLine(A, STA, number, NULL,
                    -st_lookup_mempos(Q->CurrQuad.thirdField.name, currScopeName()));
                
                fprintf(assemblycode, "LDXI %d\n", heapPointer);
                insertAssemblyLine(A, LDXI, number, NULL, heapPointer);
                
                fprintf(assemblycode, "STA 0\n");
                insertAssemblyLine(A, STA, number, NULL, 0);
                

                break;

            case IF_FALSE:
                switch (Q->CurrQuad.thirdField.op)
                {
                case IME:
                    fprintf(assemblycode, "BPL %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BPL, number, NULL, Q->CurrQuad.secField.val);

                    break;

                case MEN:
                    fprintf(assemblycode, "BEQ %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BEQ, number, NULL, Q->CurrQuad.secField.val);

                    fprintf(assemblycode, "BPL %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BPL, number, NULL, Q->CurrQuad.secField.val);
                    
                    break;

                case MAI:
                    fprintf(assemblycode, "BEQ %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BEQ, number, NULL, Q->CurrQuad.secField.val);

                    fprintf(assemblycode, "BMI %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BMI, number, NULL, Q->CurrQuad.secField.val);

                    break;

                case IMA:
                    fprintf(assemblycode, "BMI %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BMI, number, NULL, Q->CurrQuad.secField.val);

                    break;

                case IGL:
                    fprintf(assemblycode, "BNE %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BNE, number, NULL, Q->CurrQuad.secField.val);

                    break;

                case DIF:
                    fprintf(assemblycode, "BEQ %d\n", Q->CurrQuad.secField.val);
                    insertAssemblyLine(A, BEQ, number, NULL, Q->CurrQuad.secField.val);

                    break;
                
                default:
                    break;
                }

                break;

            case GOTO:
                fprintf(assemblycode, "JMP %d\n", Q->CurrQuad.firstField.val);
                insertAssemblyLine(A, JMP, number, NULL, Q->CurrQuad.firstField.val);

                break;
            
            case ASSIGN:
                switch (Q->CurrQuad.ffType)
                {
                case Var:
                    getAddress(A, Q, Var, 1);
                    fprintf(assemblycode, "LDA %d\n",
                        -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                    insertAssemblyLine(A, LDA, number, NULL, 
                        -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                    
                    break;

                case arrVar:
                    getAddress(A, Q, arrVar, 1);
                    fprintf(assemblycode, "LDA 0\n");
                    insertAssemblyLine(A, LDA, number, NULL, 0);

                    break;
                
                case constVar:
                    fprintf(assemblycode, "LDAI %d\n", Q->CurrQuad.firstField.val);
                    insertAssemblyLine(A, LDAI, number, NULL, Q->CurrQuad.firstField.val);
                    
                    break;

                case tempVar:
                    getAddress(A, Q, tempVar, 1);
                    fprintf(assemblycode, "LDA 0\n");
                    insertAssemblyLine(A, LDA, number, NULL, 0);

                    break;
                default:
                    break;
                }

                switch (Q->CurrQuad.tfType)
                {
                    case Var:
                    getAddress(A, Q, Var, 3);
                    fprintf(assemblycode, "STA %d\n",
                        -st_lookup_mempos(Q->CurrQuad.thirdField.name, currScopeName()));
                    insertAssemblyLine(A, STA, number, NULL,
                        -st_lookup_mempos(Q->CurrQuad.thirdField.name, currScopeName()));

                    break;

                case arrVar:
                    getAddress(A, Q, arrVar, 3);

                    fprintf(assemblycode, "STA 0\n");
                    insertAssemblyLine(A, STA, number, NULL, 0);

                    break;

                case tempVar:
                    getAddress(A, Q, tempVar, 3);

                    fprintf(assemblycode, "STA 0\n");
                    insertAssemblyLine(A, STA, number, NULL, 0);

                    break;
                

                default:
                    break;
                }

                break;

                case RETURNQ:

                    switch (Q->CurrQuad.ffType)
                    {
                    case Var:
                        getAddress(A, Q, Var, 1);
                        fprintf(assemblycode, "LDA %d\n", 
                            -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                        insertAssemblyLine(A, LDA, number, NULL,
                            -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                        
                        fprintf(assemblycode, "TAY\n"); //Retorno sempre estará em Y para R_PARAM pegar
                        insertAssemblyLine(A, TAY, -1, NULL, -1);
                        
                        //JMP pra fun_end
                        fprintf(assemblycode, "JMP %s\n", currScopeName());
                        insertAssemblyLine(A, JMP, string, currScopeName(), -1);
                        
                        break;

                    case arrVar:
                        getAddress(A, Q, arrVar, 1);
                        fprintf(assemblycode, "LDA 0\n");
                        insertAssemblyLine(A, LDA, number, NULL, 0);
                        
                        fprintf(assemblycode, "TAY\n"); //Retorno sempre estará em Y para R_PARAM pegar
                        insertAssemblyLine(A, TAY, -1, NULL, -1);
                        
                        //JMP pra fun_end
                        fprintf(assemblycode, "JMP %s\n", currScopeName());
                        insertAssemblyLine(A, JMP, string, currScopeName(), -1);

                        break;

                    case tempVar:
                        getAddress(A, Q, tempVar, 1);
                        fprintf(assemblycode, "LDA 0\n");
                        insertAssemblyLine(A, LDA, number, NULL, 0);
                        
                        fprintf(assemblycode, "TAY\n"); //Retorno sempre estará em Y para R_PARAM pegar
                        insertAssemblyLine(A, TAY, -1, NULL, -1);
                        
                        //JMP pra fun_end
                        fprintf(assemblycode, "JMP %s\n", currScopeName());
                        insertAssemblyLine(A, JMP, string, currScopeName(), -1);
                        break;

                    case constVar:
                        getAddress(A, Q, constVar, 1);
                        fprintf(assemblycode, "LDAI %d\n", Q->CurrQuad.firstField.val);
                        insertAssemblyLine(A, LDAI, number, NULL, Q->CurrQuad.firstField.val);
                        
                        fprintf(assemblycode, "TAY\n"); //Retorno sempre estará em Y para R_PARAM pegar
                        insertAssemblyLine(A, TAY, -1, NULL, -1);
                        
                        //JMP pra fun_end
                        fprintf(assemblycode, "JMP %s\n", currScopeName());
                        insertAssemblyLine(A, JMP, string, currScopeName(), -1);

                        break;

                    default:
                        break;
                    }

                    break;
                
                case PARAM:
                    switch (Q->CurrQuad.ffType)
                    {
                        case Var:
                            getAddress(A, Q, Var, 1);
                            fprintf(assemblycode, "LDA %d\n", 
                                -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                            insertAssemblyLine(A, LDA, number, NULL,
                                -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                            
                            fprintf(assemblycode, "PHA\n");
                            insertAssemblyLine(A, PHA, -1, NULL, -1);

                            break;
                        
                        case arrVar:
                            getAddress(A, Q, arrVar, 1);
                            
                            fprintf(assemblycode, "LDA 0\n");
                            insertAssemblyLine(A, LDA, number, NULL, 0);
                            
                            fprintf(assemblycode, "PHA\n");
                            insertAssemblyLine(A, PHA, -1, NULL, -1);

                            break;

                        case tempVar:
                            getAddress(A, Q, tempVar, 1);
                            fprintf(assemblycode, "LDA 0\n");
                            insertAssemblyLine(A, LDA, number, NULL, 0);

                            fprintf(assemblycode, "PHA\n");
                            insertAssemblyLine(A, PHA, -1, NULL, -1);

                            break;

                        case constVar:
                            fprintf(assemblycode, "LDAI %d\n", Q->CurrQuad.firstField.val);
                            insertAssemblyLine(A, LDAI, number, NULL, Q->CurrQuad.firstField.val);

                            fprintf(assemblycode, "PHA\n");
                            insertAssemblyLine(A, PHA, -1, NULL, -1);
                            break;

                        default:
                            break;
                    }

                    break;

                case LABEL:
                    insertLabel(codeLine, Q->CurrQuad.firstField.val);

                    break;

                case R_PARAM:
                    fprintf(assemblycode, "TYA\n");
                    insertAssemblyLine(A, TYA, -1, NULL, -1);

                    switch (Q->CurrQuad.ffType)
                    {
                    case Var:    
                        getAddress(A, Q, Var, 1);
                        fprintf(assemblycode, "STA %d\n", 
                            -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                        insertAssemblyLine(A, STA, number, NULL,
                            -st_lookup_mempos(Q->CurrQuad.firstField.name, currScopeName()));
                        
                        break;

                    case arrVar:
                        getAddress(A, Q, arrVar, 1);
                        fprintf(assemblycode, "STA 0\n");
                        insertAssemblyLine(A, STA, number, NULL, -1);

                        break;

                    case tempVar:
                        getAddress(A, Q, tempVar, 1);
                        fprintf(assemblycode, "STA 0\n");
                        insertAssemblyLine(A, STA, number, NULL, -1);

                        break;
                    
                    default:
                        break;
                    }

                    break;



                break;
            
            
            default:
                break;
            }


        default:
            break;
        }
    
    }

    printBinary(A);
}

assemblyListS createAssemblyList(instructionType instruction, fieldType fT, char *name, int num){
    assemblyListS A = malloc(sizeof(struct assemblyListT));

    A->instruction = instruction;
    A->fT = fT;
    if(fT == string){
        A->field.name = name;
    }else if(fT == number){
        A->field.num = num;
    }
    A->next = NULL;

    codeLine++;

    return A;
}

void insertAssemblyLine(assemblyListS A, instructionType instruction, fieldType fT, char *name, int num){
    assemblyListS aux;
    
    for(aux = A; aux->next != NULL; aux = aux->next);

    aux->next = malloc(sizeof(struct assemblyListT));
    aux->next->instruction = instruction;
    aux->next->fT = fT;
    if(fT == string){
        aux->next->field.name = name;
    }else if(fT == number){
        aux->next->field.num = num;
    }

    codeLine++;

    aux->next->next = NULL;
}

/* Decimal to binary conversion definitions */

char *decimalToBinary(int K){
    int array[26], i = 0, x;
    char *binaryNumber;

    binaryNumber = malloc(26*sizeof(char));

    for(i = 0; i < 26; i++){
        array[i] = 0;
    }

    i = 0;
    if(K >= 0){
        x = K;
    }else{
        x = -K;
    }
    for(; x > 0; x = x/2){
        array[i] = x%2;
        i++;
    }

    if(K < 0){
        //Se for negativo inverte todos os bits
        for(i = 0; i < 26; i++){
            if(array[i] == 0){
                array[i] = 1;
            }else{
                array[i] = 0;
            }
        }
        //Soma 1
        for(i = 0; i < 26; i++){
            if(array[i] == 1){
                array[i] = 0;
            }else{
                array[i] = 1;
                break;
            }
        }
        //Seta bit mais significativo
        array[25] = 1;
    }

    int j = 0;
    for(i = 25; i >= 0; i--){
        if(array[i] == 0){
            binaryNumber[j] = '0';
        }else{
            binaryNumber[j] = '1';
        }
        j++;
    }

    return binaryNumber;

}

int currBinLine = codeStart;

char *printBinInstruct(instructionType instruction){
    currBinLine++;
    switch (instruction)
    {
    case CPY:
        return "000000";
        break;

    case CPI:
        return "000001";
        break;
    
    case CMP:
        return "000010";
        break;
    
    case ASL:
        return "000011";
        break;

    case ANDI:
        return "000100";
        break;    
    
    case AND:
        return "000101";
        break;
    
    case ADCI:
        return "000110";
        break;
    
    case ADC:
        return "000111";
        break;

    case TXAX:
        return "001000";
        break;
    
    case SBCI:
        return "001001";
        break;
    
    case SBC:
        return "001010";
        break;
    
    case ORAI:
        return "001011";
        break;
    
    case ORA:
        return "001100";
        break;
    
    case LSR:
        return "001101";
        break;
    
    case EORI:
        return "001110";
        break;
    
    case EOR:
        return "001111";
        break;
    
    case TAXX:
        return "010000";
        break;
    
    case STA:
        return "010001";
        break;
    
    case LDYI:
        return "010010";
        break;
    
    case LDY:
        return "010011";
        break;
    
    case LDXI:
        return "010100";
        break;

    case LDAI:
        return "010110";
        break;

    case LDA:
        return "010111";
        break;
    case TABR:
        return "011000";
        break;
    case TXS:
        return "011001";
        break;

    case TSX:
        return "011010";
        break;

    case PLP:
        return "011011";
        break;

    case PLA:
        return "011100";
        break;

    case PHP:
        return "011101";
        break;

    case PHA:
        return "011110";
        break;

    case STY:
        return "011111";
        break;
    
    case PHPC:
        return "100000";
        break;

    case RTS:
        return "100001";
        break;

    case JSR:
        return "100010";
        break;

    case JMP:
        return "100011";
        break;

    case BPL:
        return "100100";
        break;

    case BNE:
        return "100101";
        break;
        
    case BMI:
        return "100110";
        break;

    case BEQ:
        return "100111";
        break;

    case PLPC:
        return "101000";
        break;

    case DIVSI:
        return "101001";
        break;

    case DIVS:
        return "101010";
        break;

    case MULPI:
        return "101011";
        break;

    case MULP:
        return "101100";
        break;

    case TYA:
        return "110000";
        break;

    case TXA:
        return "110001";
        break;

    case TAY:
        return "110010";
        break;

    case TAX:
        return "110011";
        break;

    case INY:
        return "110100";
        break;

    case INX:
        return "110101";
        break;

    case DEY:
        return "110110";
        break;

    case DEX:
        return "110111";
        break;

    case SPC:
        return "111000";
        break;
    
    case SFF:
        return "111001";
        break;

    case CPF:
        return "111010";
        break;

    case OUP:
        return "111011";
        break;

    case INPT:
        return "111100";
        break;

    case NOP:
        return "111110";
        break;

    case BRK:
        return "111111";
        break;

    case LDX:
        return "010101";
        break;

    default:
        return "MISSING";
        break;
    }
}

void printBinary(assemblyListS AList){
    assemblyListS A;
    binarycode = fopen("output/binarycode.txt", "w");

    for(A = AList; A != NULL; A = A->next){
        switch (A->instruction)
        {
        case TXAX:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case TAXX:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case TABR:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case PHPC:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case PHP:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case PLPC:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;
        
        case PLP:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case CPF:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case SFF:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case SPC:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case LDX:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case LDY:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;

        case ADC:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case ADCI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case CMP:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;

        case CPI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;    
        
        case SBC:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case SBCI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case MULP:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case MULPI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case DIVS:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case DIVSI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case LDA:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case LDAI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case LDXI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case STA:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(A->field.num));
            break;
        
        case PHA:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;
        
        case PLA:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;
        
        case TSX:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;
        
        case TXS:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case BEQ:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                decimalToBinary((getLabel(A->field.num)-currBinLine)-1));
            break;

        case BMI:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                decimalToBinary((getLabel(A->field.num)-currBinLine)-1));
            break;
        
        case BNE:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                decimalToBinary((getLabel(A->field.num)-currBinLine)-1));
            break;

        case BPL:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                decimalToBinary((getLabel(A->field.num)-currBinLine)-1));
            break;

        case JMP:
            switch (A->fT)
            {
            case number:
                fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                    decimalToBinary((getLabel(A->field.num))));
                break;
            case string:
                if(strcmp(A->field.name, "main") == 0){
                    fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                        decimalToBinary(st_returnFuncLine(A->field.name, 0)));
                }else{
                    fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                        decimalToBinary(st_returnFuncLine(A->field.name, 1)));
                }
                break;
            default:
                break;
            }
            break;

        case JSR:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), 
                     decimalToBinary(st_returnFuncLine(A->field.name, 0)));
            break;

        case RTS:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case TAX:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case TAY:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case TYA:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case TXA:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case BRK:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case INPT:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        case OUP:
            fprintf(binarycode, "%s%s ", printBinInstruct(A->instruction), decimalToBinary(0));
            break;

        default:
            break;
        }
    }
    
}