#ifndef ASSEMBLYGENERATE_H
#define ASSEMBLYGENERATE_H

#include "codegenerate.h"

typedef enum {string, number} fieldType;

typedef enum {ADC, ADCI, AND, ANDI, ASL, CMP, CPI, CPX, CPY, 
                EOR, EORI, LSR, ORA, ORAI, SBC, SBCI,
                MULP, MULPI, DIVS, DIVSI, LDA, LDAI, LDX, LDXI,
                LDY, LDYI, STA, STX, STY, PHA, PHP, PLA, PLP,
                TSX, TXS, BEQ, BMI, BNE, BPL, JMP, JSR, RTS, 
                CLZ, CLN, CLV, DEX, DEY, INX, INY, TAX, TAY, TXA, TYA, 
                BRK, NOP, INPT, OUP, TABR, CPF, PHPC, PLPC, SFF,
                TXAX, TAXX, SPC} instructionType;

typedef struct assemblyListT *assemblyListS;

typedef struct assemblyListT{
    instructionType instruction;
    fieldType fT;
    union {
        int num;
        char *name;
    }field;
    assemblyListS next;
}assemblyListT;

assemblyListS createAssemblyList(instructionType instruction, fieldType fT, char *name, int num);

void insertAssemblyLine(assemblyListS A, instructionType instruction, fieldType fT, char *name, int num);

void pushScopeNameStack(char *name);

void popScopeNameStack(void);

void printQList(QuadrupleList QList);

void printBinary(assemblyListS AList);

char *decimalToBinary(int K);

char *printBinInstruct(instructionType instruction);


#endif