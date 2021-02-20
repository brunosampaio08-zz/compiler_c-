
#include <stdio.h>
#include "globals.h"
#include "util.h"
#include "codegenerate.h"

/*Arquivo de saida contendo o codigo intermediario*/
FILE * icodefile;

/*Vetor para guardar a disponibilidade das variaveis temporarias*/
int tempAvailable[MAX_TEMP];

/*Contador para indentacao*/
int indentNO = 0;

/*Contador de labels*/
int labelCount = 0;

/*Primeira execução de printCode*/
int isFirst = 0;

void indent(void){
    for(int i = 0; i < indentNO; i++){
        fprintf(icodefile, "    ");
    }
}

/*Funcao para checar a menor variavel temporaria disponivel*/
int isTempAvailable(){
    for(int i = 0; i < MAX_TEMP; i++){
        if(tempAvailable[i] == 0){
            tempAvailable[i] = 1;
            return i;
        }
    }
}

/*Printa o tipo da declaracao*/
char * printType(int K){
    if(K == 0){
        return "void";
    }else{
        return "int";
    }
}

/*Funcao auxiliar que faz o print de certos tipos de nos da arvore*/
void printLine(QuadrupleList QList, TreeNode *tree){
    switch (tree->nodekind){
        /*Printa uma declaracao*/
        case DeclK:
            switch (tree->kind.decl){
                case VarK:
                    indent();
                    //Printa tipo e variavel
                    fprintf(icodefile, "(ALLOC, %s, _ ,%s)\n", printType(tree->child[0]->type) , 
                        tree->attr.name);
                    insertNQuad(QList, ALLOC, NONE, NONE, NONE, tree);

                break;
                
                //Achou um no de declaracao de array
                case ArrVarK:
                    indent();
                    fprintf(icodefile, "(ARR_ALLOC, %s, %d, %s)\n", printType(tree->child[0]->type), 
                    tree->attr.arr.size, tree->attr.arr.name);
                    insertNQuad(QList, ARR_ALLOC, NONE, NONE, NONE, tree);
                break;

                case FunK:
                    indent();
                    //Printa o retorno e o identificador da funcao
                    fprintf(icodefile, "\n(FUN_DECL, %s, _, %s)\n", printType(tree->child[0]->type), tree->attr.name);
                    //Insere a funcao na lista de quadruplas
                    insertNQuad(QList, FUN_DECL, NONE, NONE, NONE, tree);
                    
                    // //Printa todos os parametros da funcao
                    // for(TreeNode * t = tree->child[1]; t != NULL; t = t->sibling){
                    //     if(t->kind.decl == ParamK){
                    //         if(t->child[0] != NULL)
                    //             fprintf(icodefile, "(PAR_DECL, %s, _, %s)\n", printType(t->child[0]->type), 
                    //                 t->attr.name);
                    //     }else{
                    //         fprintf(icodefile, "(ARR_PAR_DECL, %s, _, %s)\n", printType(t->child[0]->type), t->attr.name);
                    //     }
                    // }
                    // fprintf(icodefile, "(END_PAR_DECL, _, _, _)\n");
                    // fprintf(icodefile, "\n");

                break;
                    
                default:
                    break;
            }
        break;
        
        case StmtK:
            switch (tree->kind.decl){
                //Todos os tratamentos sao feitos na printCode

                default: break;

            }

        break;

        case ExpK:
            switch(tree->kind.exp){

                //INCLUIR FUNCAO FREE TEMPORARY(?)
                 case CalcK:
                    //Calculo necessita de um temporario ainda nao calculado
                    if(tree->child[0]->kind.decl == CalcK){
                        printLine(QList, tree->child[0]);
                    }
                    //Calculo necessita de um temporario ainda nao calculado
                    if(tree->child[2]->kind.decl == CalcK){
                        printLine(QList, tree->child[2]);
                    }
                
                    /*Retornei das chamadas recursivas e filho esquerdo é identificador*/
                    if(tree->child[0]->kind.decl == IdK){
                        /*Filho direito e identificador*/
                        if(tree->child[2]->kind.decl == IdK){//VAR+VAR
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s, %s, _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[2]->attr.name, tree->temporary);
                            
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, Var, Var, tempVar, tree);
                        }
                        /*Filho direito é constante*/
                        else if(tree->child[2]->kind.decl == ConstK){//VAR + CONST
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s, %d, _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[2]->attr.val, tree->temporary);
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, Var, constVar, tempVar, tree);
                        }
                        /*Filho direito é um vetor*/
                        else if(tree->child[2]->kind.exp == ArrIdK){//VAR + ARR
                            /*Calcula indice do vetor*/
                            printCode(QList, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s, %s[_t%d], _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[2]->attr.name, tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, Var, arrVar, tempVar, tree);
                        }
                        /*Filho direito e um calculo ja realizado*/
                        else if(tree->child[2]->kind.exp == CalcK){
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s, _t%d, _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, Var, tempVar, tempVar, tree);
                        }else if(tree->child[2]->kind.exp == CallK){
                            tree->child[2]->temporary = isTempAvailable();
                            printCode(QList, tree->child[2]);
                            indent();
                            fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->child[2]->temporary);
                            insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s, _t%d, _t%d", tree->child[0]->attr.name,
                                tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, Var, tempVar, tempVar, tree);
                        }
                    }
                    /*Filho esquerdo e constante*/
                    else if(tree->child[0]->kind.decl == ConstK){
                        /*Filho direito e constante*/
                        if(tree->child[2]->kind.decl == ConstK){//CONST+CONST
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %d, %d, _t%d)\n",tree->child[0]->attr.val,
                                 tree->child[2]->attr.val, tree->temporary);
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, constVar, constVar, tempVar, tree);
                        }
                        /*Filho direito e identificador*/
                        else if(tree->child[2]->kind.decl == IdK){//CONST + VAR
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %d, %s, _t%d)\n",tree->child[0]->attr.val,
                                 tree->child[2]->attr.name, tree->temporary);
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, constVar, Var, tempVar, tree);
                        }
                        /*Filho direito e vetor*/
                        else if(tree->child[2]->kind.exp == ArrIdK){//CONST + ARR
                            /*Calcula indice do vetor*/
                            printCode(QList, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %d, %s[_t%d], _t%d)\n",tree->child[0]->attr.val,
                                 tree->child[2]->attr.name, tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, constVar, arrVar, tempVar, tree);
                        }
                        /*Filho direito e um calculo ja realizado*/
                        else if(tree->child[2]->kind.exp == CalcK){
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %d, _t%d, _t%d)\n",tree->child[0]->attr.val,
                                 tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, constVar, tempVar, tempVar, tree);
                        }else if(tree->child[2]->kind.exp == CallK){
                            tree->child[2]->temporary = isTempAvailable();
                            printCode(QList, tree->child[2]);
                            indent();
                            fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->child[2]->temporary);
                            insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %d, _t%d, _t%d", tree->child[0]->attr.val,
                                tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, constVar, tempVar, tempVar, tree);
                        }
                    }
                    /*Filho esquerdo e um calculo ja realizado*/
                    else if(tree->child[0]->kind.exp == CalcK){
                        /*Filho direito e um identificador*/
                        if(tree->child[2]->kind.exp == IdK){
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, %s, _t%d)\n",tree->child[0]->temporary,
                                 tree->child[2]->attr.name, tree->temporary);
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, Var, tempVar, tree);
                        }
                        /*Filho direito e constante*/
                        else if(tree->child[2]->kind.exp == ConstK){
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, %d, _t%d)\n",tree->child[0]->temporary,
                                 tree->child[2]->attr.val, tree->temporary);
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, constVar, tempVar, tree);
                        }
                        /*Filho direito e um vetor*/
                        else if(tree->child[2]->kind.exp == ArrIdK){
                            /*Calcula indice do vetor*/
                            printCode(QList, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, %s[_t%d], _t%d)\n",tree->child[0]->temporary,
                                 tree->child[2]->attr.name, tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[0]->temporary] = 0;
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, arrVar, tempVar, tree);
                        }
                        /*Filho direito e um calculo ja realizado*/
                        else if(tree->child[2]->kind.exp == CalcK){
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, _t%d, _t%d)\n",tree->child[0]->temporary,
                                 tree->child[2]->temporary, tree->temporary);

                            tempAvailable[tree->child[0]->temporary] = 0;
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, tempVar, tempVar, tree);
                        }else if(tree->child[2]->kind.exp == CallK){
                            tree->child[2]->temporary = isTempAvailable();
                            printCode(QList, tree->child[2]);
                            indent();
                            fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->child[2]->temporary);
                            insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, _t%d, _t%d)\n", tree->child[0]->temporary,
                                tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, tempVar, tempVar, tree);
                        }
                    }
                    /*Filho esquerdo e um vetor*/
                    else if(tree->child[0]->kind.exp == ArrIdK){
                        /*Calcula o indice do vetor*/
                        printCode(QList, tree->child[0]);
                        /*Filho direito e um identificador*/
                        if(tree->child[2]->kind.decl == IdK){//VAR+VAR
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s[_t%d], %s, _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[0]->temporary, tree->child[2]->attr.name, tree->temporary);
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, arrVar, Var, tempVar, tree);
                        }
                        /*Filho direito e constante*/
                        else if(tree->child[2]->kind.decl == ConstK){//VAR + CONST
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s[_t%d], %d, _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[0]->temporary, tree->child[2]->attr.val, tree->temporary);
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, arrVar, constVar, tempVar, tree);
                        }
                        /*Filho direito e um vetor*/
                        else if(tree->child[2]->kind.exp == ArrIdK){//VAR + ARR
                            printCode(QList, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s[_t%d], %s[_t%d], _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[0]->temporary, tree->child[2]->attr.name, 
                                    tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, arrVar, arrVar, tempVar, tree);
                        }
                        /*Filho direito e um calculo ja realizado*/
                        else if(tree->child[2]->kind.exp == CalcK){
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s[_t%d], _t%d, _t%d)\n",tree->child[0]->attr.name,
                                 tree->child[0]->temporary, tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, arrVar, tempVar, tempVar, tree);
                        
                        }else if(tree->child[2]->kind.exp == CallK){
                            tree->child[2]->temporary = isTempAvailable();
                            printCode(QList, tree->child[2]);
                            indent();
                            fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->child[2]->temporary);
                            insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", %s[_t%d], _t%d, _t%d)\n", tree->child[0]->attr.name,
                                tree->child[0]->temporary, tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, arrVar, tempVar, tempVar, tree);
                        }
                    }
                    //Filho esquerdo é uma chamada de funcao
                    else if(tree->child[0]->kind.exp == CallK){
                        tree->child[0]->temporary = isTempAvailable();
                        printCode(QList, tree->child[0]);
                        indent();
                        fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->child[0]->temporary);
                        insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree->child[0]);
                        if(tree->child[2]->kind.decl == IdK){//VAR+VAR
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, %s, _t%d)\n", tree->child[0]->temporary, 
                                tree->child[2]->attr.name, tree->temporary);
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, Var, tempVar, tree);
                        }
                        /*Filho direito e constante*/
                        else if(tree->child[2]->kind.decl == ConstK){//VAR + CONST
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, %d, _t%d)\n", tree->child[0]->temporary, 
                                tree->child[2]->attr.val, tree->temporary);
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, constVar, tempVar, tree);
                        }
                        /*Filho direito e um vetor*/
                        else if(tree->child[2]->kind.exp == ArrIdK){//VAR + ARR
                            printCode(QList, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, %s[_t%d], _t%d)\n", tree->child[0]->temporary, 
                                tree->child[2]->attr.name, tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, arrVar, tempVar, tree);
                        }
                        /*Filho direito e um calculo ja realizado*/
                        else if(tree->child[2]->kind.exp == CalcK){
                            tree->temporary = isTempAvailable();
                            indent();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, _t%d, _t%d)\n", tree->child[0]->temporary, 
                                tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, tempVar, tempVar, tree);
                        }else if(tree->child[2]->kind.exp == CallK){
                            tree->child[2]->temporary = isTempAvailable();
                            printCode(QList, tree->child[2]);
                            indent();
                            fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->child[2]->temporary);
                            insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree->child[2]);
                            tree->temporary = isTempAvailable();
                            fprintf(icodefile, "(");
                            printToken(2, tree->child[1]->attr.op, NULL);
                            fprintf(icodefile, ", _t%d, _t%d, _t%d)\n", tree->child[0]->temporary, 
                                tree->child[2]->temporary, tree->temporary);
                            tempAvailable[tree->child[2]->temporary] = 0;
                            tempAvailable[tree->child[0]->temporary] = 0;
                            insertCalcQuadruple(QList, tree->child[1]->attr.op, tempVar, tempVar, tempVar, tree);
                        }

                        tempAvailable[tree->child[0]->temporary] = 0;
                    }
                    
                    
                break;

                case IdK:
                    //Imprime o identificador
                    fprintf(icodefile, "%s", tree->attr.name);
                break;

                case ConstK:
                    //Imprime a constante
                    fprintf(icodefile, "%d", tree->attr.val);

                break;

                case ArrIdK:
                    
                break;

            }
    
        default:
            break;
    }

}

void printCode(QuadrupleList QList,  TreeNode * tree )
{ 
    int i, localLabel = 0;

    switch(tree->nodekind){
        case DeclK:
            switch(tree->kind.decl){
                //Achou uma funcao: 
                case FunK:
                    ;
                    //Verifica se todos os parametros globais ja foram impressos
                    TreeNode *t;
                    
                    for(t = tree; t->sibling != NULL; t = t->sibling){
                        if(t->sibling->kind.decl == VarK || t->sibling->kind.decl == ArrVarK){
                            printLine(QList, t->sibling);
                            t->sibling = t->sibling->sibling;
                        }
                    }

                    if(isFirst == 0){
                        //Se e a primeira funcao q achou, pula pra executar a main
                        isFirst = 1;
                        fprintf(icodefile, "(CALL, main, _, _)\n");
                        TreeNode *mainT;
                        mainT = malloc(sizeof(TreeNode));
                        mainT->attr.name = "main";
                        insertNQuad(QList, CALL, NONE, NONE, NONE, mainT);
                    }

                    //Printa a funcao
                    printLine(QList, tree);
                    //Funcao tem corpo?
                    indentNO+=1;
                    if(tree->child[2] != NULL){
                        printCode(QList, tree->child[2]);
                    }
                    indentNO-=1;

                    indent();
                    fprintf(icodefile, "(FUN_END, _, _, _)");
                    insertNQuad(QList, FUN_END, NONE, NONE, NONE, tree);
                    
                    //Funcao tem irmao?
                    
                    if(tree->sibling != NULL){
                        printCode(QList, tree->sibling);
                    }
                    
                    break;
                //Achou uma declaracao de variavel
                case VarK:
                    printLine(QList, tree);
                    //Variavel tem irmao?
                    if(tree->sibling != NULL){
                        printCode(QList, tree->sibling);
                    }
                break;
                
                case ArrVarK:
                    printLine(QList, tree);
                    //Variavel tem irmao?
                    if(tree->sibling != NULL){
                        printCode(QList, tree->sibling);
                    }
                break;
            }
        break;

        case StmtK:
            switch(tree->kind.stmt){
                case CompoundK:
                    //Achou uma declaracao composta:
                    //indentNO+=1;
                    //Printa as declaracoes, se existirem
                    if(tree->child[0] != NULL)
                        printCode(QList, tree->child[0]);

                    //Printa o corpo da declaracao, se existir
                    for(TreeNode *t = tree->child[1]; t != NULL; t = t->sibling){
                        printCode(QList, t);
                    }
                    //indentNO-=1;
                    
                break;
                
                //Achou uma selecao declaracao:
                case IfK:
                    //Printa os calculos que levarão as condicoes para que if seja aceito
                    printCode(QList, tree->child[0]);
                    indent();
                    //Se negacao da condicao, pula o corpo do if
                    fprintf(icodefile, "(IF_FALSE, _t%d, _L%d, _)\n\n", tree->child[0]->temporary, labelCount);
                    insertNQuad(QList, IF_FALSE, NONE, NONE, NONE, tree);
                    labelCount++;
                    tempAvailable[tree->child[0]->temporary] = 0;
                    indentNO+=1;
                    //Printa o corpo do if, se existir

                    localLabel = labelCount;
                    labelCount++;
                    if(tree->child[1] != NULL){
                        printCode(QList, tree->child[1]);
                    }
                    indentNO-=1;
                    
                    labelCount = localLabel;
                    if(tree->child[2] != NULL){
                        indent();
                        fprintf(icodefile, "(GOTO, _, _L%d, _)\n", labelCount);
                        insertNQuad(QList, GOTO, NONE, NONE, NONE, NULL);
                        indent();
                        //Final do if
                        fprintf(icodefile, "(LABEL, _L%d, _, _)\n\n", labelCount-1);
                        labelCount--;
                        insertNQuad(QList, LABEL, NONE, NONE, NONE, NULL);
                        labelCount++;
                        printCode(QList, tree->child[2]);
                        indent();
                        fprintf(icodefile, "(LABEL, _L%d, _, _)\n", labelCount);
                        insertNQuad(QList, LABEL, NONE, NONE, NONE, NULL);
                    }else{
                        indent();
                        //Final do if
                        fprintf(icodefile, "(LABEL, _L%d, _, _)\n\n", labelCount-1);
                        labelCount--;
                        insertNQuad(QList, LABEL, NONE, NONE, NONE, NULL);
                        labelCount++;
                    }
                break;

                //Achou uma iteracao declaracao
                case WhileK:
                    fprintf(icodefile, "\n");
                    indent();
                    fprintf(icodefile, "(LABEL, _L%d, _, _)\n", labelCount);
                    insertNQuad(QList, LABEL, NONE, NONE, NONE, NULL);
                    labelCount++;
                    //Printa os calculos que levarao a aceitacao da condicao
                    printLine(QList, tree->child[0]);
                    
                    indent();
                    //Se negacao da condicao, pula o corpo do while
                    fprintf(icodefile, "(IF_FALSE, _t%d, L%d, _)\n\n", tree->child[0]->temporary, labelCount);
                    insertNQuad(QList, IF_FALSE, NONE, NONE, NONE, tree);
                    tempAvailable[tree->child[0]->temporary] = 0;
                    indentNO+=1;
                    //Printa o corpo do while se existir
                    localLabel = labelCount;
                    labelCount++;
                    if(tree->child[1] != NULL){
                        printCode(QList, tree->child[1]);
                    }
                    labelCount = localLabel;
                    indentNO-=1;
                    //Fim do while(volta para a checagem)
                    indent();
                    labelCount--;
                    fprintf(icodefile, "(GOTO, L%d, _, _)\n", labelCount);
                    //Insere qudrupla
                    insertNQuad(QList, GOTO, NONE, NONE, NONE, NULL);
                    labelCount++;
                    indent();
                    fprintf(icodefile, "(LABEL, _L%d, _, _)\n\n", labelCount);
                    insertNQuad(QList, LABEL, NONE, NONE, NONE, NULL);

                break;

                //Achou uma atribuicao declaracao
                case AssignK:

                    //Quem esta recebendo e um identificador?
                    if(tree->child[0]->kind.exp == IdK){
                        //Esta atribuindo um calculo?
                        if(tree->child[1]->kind.exp == CalcK){
                            printCode(QList, tree->child[1]);
                            indent();
                            fprintf(icodefile, "(ASSIGN, _t%d, _, %s)\n", tree->child[1]->temporary, tree->child[0]->attr.name);
                            tempAvailable[tree->child[1]->temporary] = 0;
                            insertNQuad(QList, ASSIGN, tempVar, NONE, Var, tree);
                        }
                        //Esta atribuindo um identificador?
                        else if(tree->child[1]->kind.exp == IdK){
                            indent();
                            fprintf(icodefile, "(ASSIGN, %s, _, %s)\n", tree->child[1]->attr.name, 
                                tree->child[0]->attr.name);
                            insertNQuad(QList, ASSIGN, Var, NONE, Var, tree);
                        }
                        //Esta atribuindo um retorno de funcao?
                        else if(tree->child[1]->kind.exp == CallK){
                            printCode(QList, tree->child[1]);
                            indent();
                            fprintf(icodefile, "(R_PARAM, _, _, %s)\n", tree->child[0]->attr.name);
                            insertNQuad(QList, R_PARAM, Var, NONE, NONE, tree->child[0]);
                        }
                        //Esta atribuindo um vetor?
                        else if(tree->child[1]->kind.exp == ArrIdK){
                            printCode(QList, tree->child[1]);
                            indent();
                            fprintf(icodefile, "(ASSIGN, %s[_t%d], _, %s)\n", tree->child[1]->attr.name,
                                tree->child[1]->temporary, tree->child[0]->attr.name);
                            tempAvailable[tree->child[1]->temporary] = 0;
                            insertNQuad(QList, ASSIGN, arrVar, NONE, Var, tree);
                        }
                        //Esta atribuindo uma constante
                        else if(tree->child[1]->kind.exp == ConstK){
                            indent();
                            fprintf(icodefile, "(ASSIGN, %d, _, %s)\n", tree->child[1]->attr.val, 
                                tree->child[0]->attr.name);
                            insertNQuad(QList, ASSIGN, constVar, NONE, Var, tree);
                        }
                    }
                    //Quem esta recebendo e um vetor?
                    else if(tree->child[0]->kind.exp == ArrIdK){
                        printCode(QList, tree->child[0]);
                        //Esta atribuindo um calculo?
                        if(tree->child[1]->kind.exp == CalcK){
                            printCode(QList, tree->child[1]);
                            indent();
                            fprintf(icodefile, "(ASSIGN, _t%d, _, %s[_t%d])\n", tree->child[1]->temporary, 
                                tree->child[0]->attr.name, tree->child[0]->temporary);
                            tempAvailable[tree->child[1]->temporary] = 0;
                            insertNQuad(QList, ASSIGN, tempVar, NONE, arrVar, tree);
                        }
                        //Esta atribuindo um identificador?
                        else if(tree->child[1]->kind.exp == IdK){
                            indent();
                            fprintf(icodefile, "(ASSIGN, %s, _, %s[_t%d])\n", tree->child[1]->attr.name, 
                                tree->child[0]->attr.name, tree->child[0]->temporary);
                            insertNQuad(QList, ASSIGN, Var, NONE, arrVar, tree);
                        }
                        //Esta atribuindo um retorno de funcao?
                        else if(tree->child[1]->kind.exp == CallK){
                            printCode(QList, tree->child[1]);
                            indent();
                            fprintf(icodefile, "(R_PARAM, _, _, %s[_t%d])\n", tree->child[0]->attr.name,
                                tree->child[0]->temporary);
                            insertNQuad(QList, R_PARAM, arrVar, NONE, NONE, tree->child[0]);
                        }
                        //Esta atribuindo um vetor?
                        else if(tree->child[1]->kind.exp == ArrIdK){
                            printCode(QList, tree->child[1]);
                            indent();
                            fprintf(icodefile, "(ASSIGN, %s[_t%d], _, %s[_t%d])\n", tree->child[1]->attr.name,
                                tree->child[1]->temporary, tree->child[0]->attr.name, tree->child[0]->temporary);
                            tempAvailable[tree->child[1]->temporary] = 0;
                            insertNQuad(QList, ASSIGN, arrVar, NONE, arrVar, tree);
                        }
                        //Esta atribuindo uma constante?
                        else if(tree->child[1]->kind.exp == ConstK){
                            indent();
                            fprintf(icodefile, "(ASSIGN, %d, _, %s[_t%d])\n", tree->child[1]->attr.val, 
                                tree->child[0]->attr.name, tree->child[0]->temporary);
                            insertNQuad(QList, ASSIGN, constVar, NONE, arrVar, tree);
                        }
                        tempAvailable[tree->child[0]->temporary] = 0;
                    }
                break;

                //Achou um no de return
                case ReturnK:
                    //Retorno nao vazio
                    if(tree->child[0] != NULL){
                        //Se o retorno e uma atribuicao
                        if(tree->child[0]->nodekind == StmtK){
                            if(tree->child[0]->kind.stmt == AssignK){
                                //Calcula a expressao de retorno
                                printCode(QList, tree->child[0]);
                                indent();
                                fprintf(icodefile, "(RETURN, %s, _, _)\n", tree->child[0]->attr.name);
                                insertNQuad(QList, RETURNQ, Var, NONE, NONE, tree);
                            }
                        }else if(tree->child[0]->nodekind == ExpK){
                            if(tree->child[0]->kind.exp == IdK){
                                indent();
                                fprintf(icodefile, "(RETURN, %s, _, _)\n", tree->child[0]->attr.name);
                                insertNQuad(QList, RETURNQ, Var, NONE, NONE, tree);
                            }else if(tree->child[0]->kind.exp == ConstK){
                                indent();
                                fprintf(icodefile, "(RETURN, %d, _, _)\n", tree->child[0]->attr.val);
                                insertNQuad(QList, RETURNQ, constVar, NONE, NONE, tree);
                            }
                            //Se o retorno e um calculo
                            else if(tree->child[0]->kind.exp == CalcK){
                                //Calcula a expressao de retorno
                                printCode(QList, tree->child[0]);
                                indent();
                                fprintf(icodefile, "(RETURN, _t%d, _, _)\n", tree->child[0]->temporary);
                                insertNQuad(QList, RETURNQ, tempVar, NONE, NONE, tree);
                                tempAvailable[tree->child[0]->temporary] = 0;
                            }else if(tree->child[0]->kind.exp == CallK){
                                    tree->temporary = isTempAvailable();
                                    printCode(QList, tree->child[0]);
                                    indent();
                                    fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->temporary);
                                    insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree);
                                    indent();
                                    fprintf(icodefile, "(RETURN, _t%d, _, _)\n", tree->temporary);
                                    insertNQuad(QList, RETURNQ, tempVar, NONE, NONE, tree);
                                    tempAvailable[tree->temporary] = 0;
                            }
                        }
                    }
                    //Retorno vazio
                    else{
                        indent();
                        fprintf(icodefile, "(RETURN, _, _, _)\n");
                        insertNQuad(QList, RETURNQ, NONE, NONE, NONE, tree);
                    }

                break;
            }
        
        break;
        
        case ExpK:
            switch(tree->kind.exp){
                
                //Achou um no de calculo
                case CalcK:

                    printLine(QList, tree);
                
                break;

                //Achou um no de chamada de funcao
                case CallK:
                    //Imprime os parametros da funcao, se existirem
                    for(TreeNode * t = tree->child[0]; t != NULL; t = t-> sibling){
                            
                            //Parametro e identificador ou constante?
                            if(t->kind.exp == IdK || t->kind.exp == ConstK){
                                indent();
                                fprintf(icodefile, "(PARAM, ");
                                printLine(QList, t);
                                fprintf(icodefile, ", _, _)\n");
                                if(t->kind.exp == IdK){
                                    insertNQuad(QList, PARAM, Var, NONE, NONE, t);
                                }else{
                                    insertNQuad(QList, PARAM, constVar, NONE, NONE, t);
                                }
                            }
                            //Parametro e um calculo?
                            else if(t->kind.exp == CalcK){
                                printCode(QList, t);
                                indent();
                                fprintf(icodefile, "(PARAM, _t%d, _, _)\n", t->temporary);
                                tempAvailable[t->temporary] = 0;
                                insertNQuad(QList, PARAM, tempVar, NONE, NONE, t);
                            }
                            //Parametro e uma atribuicao?
                            else if(t->kind.exp == AssignK){
                                printCode(QList, t);
                                indent();
                                fprintf(icodefile, "(PARAM, %s, _, _)\n", t->attr.name);
                                insertNQuad(QList, PARAM, Var, NONE, NONE, t);
                            }
                            //Parametro e um vetor?
                            else if(t->kind.exp == ArrIdK){
                                printCode(QList, t);
                                indent();
                                fprintf(icodefile, "(PARAM, %s[_t%d], _, _)\n", t->attr.name, t->temporary);    
                                insertNQuad(QList, PARAM, arrVar, NONE, NONE, t);
                            }else if(t->kind.exp == CallK){
                                t->temporary = isTempAvailable();
                                printCode(QList, t);
                                indent();
                                fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", t->temporary);
                                insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, t);
                                indent();
                                fprintf(icodefile, "(PARAM, _t%d, _, _)\n", t->temporary);
                                insertNQuad(QList, PARAM, tempVar, NONE, NONE, t);
                            }
                    }
                    for(TreeNode * t = tree->child[0]; t != NULL; t = t-> sibling){
                        if(tempAvailable[t->temporary] == 1){
                            tempAvailable[t->temporary] == 0;
                        }
                    }
                    indent();
                    fprintf(icodefile, "(CALL, %s, _, _)\n",tree->attr.name);
                    insertNQuad(QList, CALL, NONE, NONE, NONE, tree);
                break;

                case ArrIdK:
                    //Indice e um identificador
                    if(tree->child[0]->kind.exp == IdK){
                        tree->temporary = isTempAvailable();
                        indent();
                        fprintf(icodefile, "(ASSIGN, %s, _, _t%d)\n", tree->child[0]->attr.name, tree->temporary);
                        insertNQuad(QList, ASSIGN, Var, NONE, tempVar, tree);
                    }
                    //Indice e uma constante
                    else if(tree->child[0]->kind.exp == ConstK){
                        tree->temporary = isTempAvailable();
                        indent();
                        fprintf(icodefile, "(ASSIGN, %d, _, _t%d)\n", tree->child[0]->attr.val, tree->temporary);
                        insertNQuad(QList, ASSIGN, constVar, NONE, tempVar, tree);
                    }
                    //Indice e uma atribuicao
                    else if(tree->child[0]->kind.stmt == AssignK){
                        printCode(QList, tree->child[0]);
                        tree->temporary = isTempAvailable();
                        indent();
                        if(tree->child[0]->child[0]->kind.exp == IdK){
                            fprintf(icodefile, "(ASSIGN, %s, _, _t%d)\n", tree->child[0]->child[0]->attr.name,
                                 tree->temporary);
                            insertNQuad(QList, ASSIGN, Var, Var, tempVar, tree);
                            
                        }else if(tree->child[0]->child[0]->kind.exp == ArrIdK){
                            fprintf(icodefile, "(ASSIGN, %s[_t%d], _, _t%d)\n", tree->child[0]->child[0]->attr.name, 
                                tree->child[0]->child[0]->temporary, tree->temporary);
                            insertNQuad(QList, ASSIGN, arrVar, arrVar, tempVar, tree);
                        }
                    }
                    //Indice e uma chamada de funcao
                    else if(tree->child[0]->kind.exp == CallK){
                        tree->temporary = isTempAvailable();
                        printCode(QList, tree->child[0]);
                        indent();
                        fprintf(icodefile, "(R_PARAM, _, _, _t%d)\n", tree->temporary);
                        insertNQuad(QList, R_PARAM, tempVar, NONE, NONE, tree);
                    }
                    //Indice e um calculo
                    else if(tree->child[0]->kind.exp == CalcK){
                        tree->temporary = isTempAvailable();
                        printCode(QList, tree->child[0]);
                        indent();
                        fprintf(icodefile, "(ASSIGN, _t%d, _, _t%d)\n", tree->child[0]->temporary, 
                            tree->temporary);
                        insertNQuad(QList, ASSIGN, tempVar, NONE, tempVar, tree);
                        tempAvailable[tree->child[0]->temporary] = 0;
                    }
                break;
            }
        
        break;

        default:
            break;

    }
        
}

void insertCalcQuadruple(QuadrupleList QList, TokenType op, varFieldType f1,
        varFieldType f2, varFieldType f3, TreeNode * t){

    QuadrupleList Q;

    for(Q = QList; Q->next != NULL; Q = Q->next);

    Q->next = malloc(sizeof(struct QuadrupleListT));

    Q->next->CurrQuad.Lab = CalcQuad;
    Q->next->CurrQuad.QuadT.TType = op; 
    Q->next->CurrQuad.ffType = f1;
    Q->next->CurrQuad.sfType = f2;
    Q->next->CurrQuad.tfType = f3;

    switch (f1)
    {
    case Var:
        Q->next->CurrQuad.firstField.name = t->child[0]->attr.name;
        break;
    
    case arrVar:
        Q->next->CurrQuad.firstField.arr.name = t->child[0]->attr.name;
        Q->next->CurrQuad.firstField.arr.arrNum = t->child[0]->temporary;
        break;

    case tempVar:
        Q->next->CurrQuad.firstField.val = t->child[0]->temporary;
        break;

    case constVar:
        Q->next->CurrQuad.firstField.val = t->child[0]->attr.val;
        break;

    default:
        break;
    }

    switch (f2)
    {
    case Var:
        Q->next->CurrQuad.secField.name = t->child[2]->attr.name;
        break;
    
    case arrVar:
        Q->next->CurrQuad.secField.arr.name = t->child[2]->attr.name;
        Q->next->CurrQuad.secField.arr.arrNum = t->child[2]->temporary;
        break;

    case tempVar:
        Q->next->CurrQuad.secField.val = t->child[2]->temporary;
        break;
    
    case constVar:
        Q->next->CurrQuad.secField.val = t->child[2]->attr.val;
        break;

    default:
        break;
    }

    switch (f3)
    {
    
    case tempVar:
        Q->next->CurrQuad.thirdField.val = t->temporary;
        break;

    default:
        break;
    }

    
}

Quadruple insertNQuad(QuadrupleList QList, QuadrupleType QType, varFieldType f1,
        varFieldType f2, varFieldType f3, TreeNode * t){

    QuadrupleList Q;

    for(Q = QList; Q->next != NULL; Q = Q->next);

    Q->next = malloc(sizeof(struct QuadrupleListT));

    Q->next->CurrQuad.Lab = NQuad;
    Q->next->CurrQuad.QuadT.QType = QType; 
    Q->next->CurrQuad.ffType = f1;
    Q->next->CurrQuad.sfType = f2;
    Q->next->CurrQuad.tfType = f3;

    switch (QType)
    {
        case ALLOC:
        case FUN_DECL:
            Q->next->CurrQuad.thirdField.name = t->attr.name;
            break;

        case ARR_ALLOC:
            Q->next->CurrQuad.secField.val = t->attr.arr.size;
            Q->next->CurrQuad.thirdField.name = t->attr.arr.name;
            break;
        
        case IF_FALSE:
            Q->next->CurrQuad.firstField.val = t->child[0]->temporary;
            Q->next->CurrQuad.secField.val = labelCount;
            Q->next->CurrQuad.thirdField.op = t->child[0]->child[1]->attr.op;
            break;

        case GOTO:
            Q->next->CurrQuad.firstField.val = labelCount;
            break;
        
        case CALL:
            Q->next->CurrQuad.firstField.name = t->attr.name;
            break;

        case ASSIGN:
            switch (f3)
            {
                case Var:
                    Q->next->CurrQuad.thirdField.name = t->child[0]->attr.name;
                    switch (f1)
                    {
                        case Var:
                            Q->next->CurrQuad.firstField.name = t->child[1]->attr.name;
                        break;
                    
                        case arrVar:
                            Q->next->CurrQuad.firstField.arr.name = t->child[1]->attr.name;
                            Q->next->CurrQuad.firstField.arr.arrNum = t->child[1]->temporary;
                        break;

                        case tempVar:
                            Q->next->CurrQuad.firstField.val = t->child[1]->temporary;
                        break;

                        case constVar:
                            Q->next->CurrQuad.firstField.val = t->child[1]->attr.val;
                            break;

                        default:
                            break;
                    }
                    
                    break;
                
                case arrVar:
                    Q->next->CurrQuad.thirdField.arr.name = t->child[0]->attr.name;
                    Q->next->CurrQuad.thirdField.arr.arrNum = t->child[0]->temporary;
                    
                    switch (f1)
                    {
                        case Var:
                            Q->next->CurrQuad.firstField.name = t->child[1]->attr.name;
                        break;
                    
                        case arrVar:
                            Q->next->CurrQuad.firstField.arr.name = t->child[1]->attr.name;
                            Q->next->CurrQuad.firstField.arr.arrNum = t->child[1]->temporary;
                        break;

                        case tempVar:
                            Q->next->CurrQuad.firstField.val = t->child[1]->temporary;
                        break;

                        case constVar:
                            Q->next->CurrQuad.firstField.val = t->child[1]->attr.val;
                            break;

                        default:
                            break;
                    }
                    
                    break;
                
                case tempVar:
                    Q->next->CurrQuad.thirdField.val = t->temporary;
                    
                    switch (f2)
                    {
                    case Var:
                        Q->next->CurrQuad.firstField.name = t->child[0]->child[0]->attr.name;
                        break;
                    case arrVar:
                        Q->next->CurrQuad.firstField.arr.name = t->child[0]->child[0]->attr.name;
                        Q->next->CurrQuad.firstField.arr.arrNum = t->child[0]->child[0]->temporary; 
                        break;
                    
                    case NONE:
                        switch (f1)
                        {
                            case Var:
                                Q->next->CurrQuad.firstField.name = t->child[0]->attr.name;
                            break;
                        
                            case arrVar:
                                Q->next->CurrQuad.firstField.arr.name = t->child[0]->attr.name;
                                Q->next->CurrQuad.firstField.arr.arrNum = t->child[0]->temporary;
                            break;

                            case tempVar:
                                Q->next->CurrQuad.firstField.val = t->child[0]->temporary;
                            break;

                            case constVar:
                                Q->next->CurrQuad.firstField.val = t->child[0]->attr.val;
                            break;

                            default:
                            break;
                        }    
                        break;
                    }

                    break;

                default:
                    break;
            }

            
            break;

        case RETURNQ:
            switch (f1)
            {
                case Var:
                    Q->next->CurrQuad.firstField.name = t->child[0]->attr.name;
                break;
            
                case arrVar:
                    Q->next->CurrQuad.firstField.arr.name = t->child[0]->attr.name;
                    Q->next->CurrQuad.firstField.arr.arrNum = t->child[0]->temporary;
                break;

                case tempVar:
                    Q->next->CurrQuad.firstField.val = t->child[0]->temporary;
                break;

                case constVar:
                    Q->next->CurrQuad.firstField.val = t->child[0]->attr.val;
                    break;

                default:
                    break;
            }
            break;
        
        case PARAM:
            switch (f1)
            {
                case Var:
                    Q->next->CurrQuad.firstField.name = t->attr.name;
                    break;
            
                case arrVar:
                    Q->next->CurrQuad.firstField.arr.name = t->attr.name;
                    Q->next->CurrQuad.firstField.arr.arrNum = t->temporary;
                    break;

                case tempVar:
                    Q->next->CurrQuad.firstField.val = t->temporary;
                    break;

                case constVar:
                    Q->next->CurrQuad.firstField.val = t->attr.val;
                    break;

                default:
                    break;
            }
            break;


        case R_PARAM:
            switch (f1)
            {
                case Var:
                    Q->next->CurrQuad.firstField.name = t->attr.name;
                break;
            
                case arrVar:
                    Q->next->CurrQuad.firstField.arr.name = t->attr.name;
                    Q->next->CurrQuad.firstField.arr.arrNum = t->temporary;
                break;

                case tempVar:
                    Q->next->CurrQuad.firstField.val = t->temporary;
                break;
                default:

                break;
            }
            break;

        case LABEL:
            Q->next->CurrQuad.firstField.val = labelCount;

            break;

        case FUN_END:
            Q->next->CurrQuad.thirdField.name = t->attr.name;

        default:
            break;
    }

}

