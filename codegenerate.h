#ifndef _CODEGENERATE_H_
#define _CODEGENERATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * DEFINICAO E FUNCOES DAS QUADRUPLAS
*/

/*Tamanho maximo do nome de uma quadrupla*/
#define MAX_Q_SIZE 12
typedef enum {Var, arrVar, tempVar, constVar, label, NONE} varFieldType;
typedef enum {FUN_DECL, FUN_END, ALLOC, ARR_ALLOC, IF_FALSE,
    GOTO, CALL, ASSIGN, PARAM, R_PARAM, RETURNQ, LABEL} QuadrupleType;
typedef enum {CalcQuad, NQuad} QuadLab;

typedef struct arrT{
    int arrNum;
    char * name;
}arrT;

/*Estrutura de uma quádrupla*/
typedef struct QuadrupleT{
    QuadLab Lab;
    
    union { 
        QuadrupleType QType;
        TokenType TType;
    } QuadT;
    
    varFieldType ffType, sfType, tfType;

    union{
        char * name;
        arrT arr;
        int val;
        TokenType op;
    }firstField, secField, thirdField;
}Quadruple;

typedef struct QuadrupleListT{
    Quadruple CurrQuad;
    struct QuadrupleListT * next;
} * QuadrupleList;

void insertCalcQuadruple(QuadrupleList QList, TokenType op, varFieldType f1,
        varFieldType f2, varFieldType f3, TreeNode * t);

Quadruple insertNQuad(QuadrupleList QList, QuadrupleType QType, varFieldType f1,
        varFieldType f2, varFieldType f3, TreeNode * t);

/*
 * DEFINICAO E FUNCOES DE ESCOPO AUXILIARES A IMPRESSAO DO BINARIO
*/

void printCode(QuadrupleList QList, TreeNode *tree);
void printLine(QuadrupleList QList, TreeNode *tree);

#endif