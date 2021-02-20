%{
    #define YYPARSER

    #include "globals.h"
    #include "util.h"
    #include "scan.h"
    #include "parse.h"
    #define YYSTYPE TreeNode *
    static char * savedName; //names for use
    static int savedLineNo; //line number for use
    static TreeNode * savedTree; //tree to be returned
    static int savedNumber; //value for use

    char tokenString[MAXTOKENLEN+1];

    static int yylex(void);
    int yyerror(char * message);
%}

//Definição dos tokens

%token ELSE IF INT RETURN VOID WHILE NUM ID SOM SUB MUL DIV MEN IME MAI IMA IGL DIF
%token ATR PEV VIR APR FPR ACL FCL ACH FCH FIM ENT TAB ERR


//Gramática de C-
%%

programa: declaracao-lista{
            savedTree = $1;
        };

declaracao-lista: declaracao-lista declaracao{ 
                    YYSTYPE t = $1;
                     if (t != NULL) {
                       while (t->sibling != NULL) { t = t->sibling; }
                       t->sibling = $2;
                       $$ = $1;
                     } else {
                       $$ = $2;
                     }
                   } 
                   | declaracao{
                       $$ = $1;
                   };

declaracao: var-declaracao{
                $$ = $1;
            } 
            | fun-declaracao{
                $$ = $1;
            };

var-declaracao: tipo-especificador id PEV{ 
                    $$ = newDeclNode(VarK);
                    $$->child[0] = $1;
                    $$->lineno = lineno;
                    $$->attr.name = savedName;
                  } 
                  | tipo-especificador id ACL num FCL PEV{
                    $$ = newDeclNode(ArrVarK);
                    $$->child[0] = $1;
                    $$->lineno = lineno;
                    $$->type = IntegerArray;
                    $$->attr.arr.name = savedName;
                    $$->attr.arr.size = savedNumber;
                  };

tipo-especificador: INT{
                        $$ = newExpNode(TypeK);
                        $$->type = Integer;
                    } 
                    | VOID{
                        $$ = newExpNode(TypeK);
                        $$->type = Void;
                    };

fun-declaracao: tipo-especificador id {
                                        $$ = newDeclNode(FunK);
                                        $$->lineno = lineno;
                                        $$->attr.name = savedName;
                                    } APR params FPR composto-decl{
                                        $$ = $3;
                                        $$->child[0] = $1;
                                        $$->child[1] = $5;
                                        $$->child[2] = $7;
                                    };

params: param-lista{
            $$ = $1;
        } | VOID{
            $$ = newDeclNode(ParamK);
            $$->type = Void;
        };

param-lista: param-lista VIR param{
                YYSTYPE t = $1;
                if (t != NULL) {
                    while (t->sibling != NULL){
                        t = t->sibling; 
                    }
                    t->sibling = $3;
                    $$ = $1;
                } else {
                    $$ = $3;
                }
            } | param{
                $$ = $1;
            };

param: tipo-especificador id{
            $$ = newDeclNode(ParamK);
            $$->child[0] = $1;
            $$->attr.name = savedName;
        } | tipo-especificador id ACL FCL{
            $$ = newDeclNode(ArrParamK);
            $$->child[0] = $1;
            $$->attr.name = copyString(savedName);
        };

composto-decl: ACH local-declaracoes statement-lista FCH{
                    $$ = newStmtNode(CompoundK);
                    $$->child[0] = $2;
                    $$->child[1] = $3;
                };

local-declaracoes: local-declaracoes var-declaracao{
                        YYSTYPE t = $1;
                        if (t != NULL) {
                            while (t->sibling != NULL){
                                t = t->sibling; 
                            }
                            t->sibling = $2;
                            $$ = $1;
                        } else {
                            $$ = $2;
                        }
                    } | {
                        $$ = NULL;
                    };

statement-lista: statement-lista statement{
                    YYSTYPE t = $1;
                    if (t != NULL) {
                        while (t->sibling != NULL) { 
                            t = t->sibling; 
                        }
                    t->sibling = $2;
                    $$ = $1;
                    } else {
                        $$ = $2;
                    }
                } | {
                    $$ = NULL;
                } | error {yyerrok;};

statement: expressao-decl{
                $$ = $1;
            } | composto-decl{
                $$ = $1;
            } | selecao-decl{
                $$ = $1;
            } | iteracao-decl{
                $$ = $1;
            } | retorno-decl{
                $$ = $1;
            };

expressao-decl: expressao PEV{
                    $$ = $1;
                } | PEV{
                    $$ = NULL;
                };

selecao-decl: IF APR expressao FPR statement{
                $$ = newStmtNode(IfK);
                $$->child[0] = $3;
                $$->child[1] = $5;
            } | IF APR expressao FPR statement ELSE statement{
                $$ = newStmtNode(IfK);
                $$->child[0] = $3;
                $$->child[1] = $5;
                $$->child[2] = $7;
            };

iteracao-decl: WHILE APR expressao FPR statement{
                    $$ = newStmtNode(WhileK);
                    $$->child[0] = $3;
                    $$->child[1] = $5;
                };

retorno-decl: RETURN PEV{
                $$ = newStmtNode(ReturnK);
                $$->type = Void;
            } | RETURN expressao PEV{
                $$ = newStmtNode(ReturnK);
                $$->child[0] = $2;
            };

expressao: var ATR expressao{   
                $$ = newStmtNode(AssignK);
                $$->child[0] = $1;
                $$->child[1] = $3;
            } | simples-expressao{
                $$ = $1;
            };

var: id {
        $$ = newExpNode(IdK);
        $$->attr.name = savedName;
    } | id{
        $$ = newExpNode(ArrIdK);
        $$->attr.name = savedName;
    } ACL expressao FCL{
        $$ = $2;
        $$->child[0] = $4;
    };

simples-expressao: soma-expressao relacional soma-expressao{
                        $$ = newExpNode(CalcK);
                        $$->child[0] = $1;
                        $$->child[1] = $2;
                        $$->child[2] = $3;
                    } | soma-expressao{
                        $$ = $1;
                    };

relacional: IME{
                $$ = newExpNode(OpK);
                $$->attr.op = IME;
            } | MEN{
                $$ = newExpNode(OpK);
                $$->attr.op = MEN;
            } | MAI{
                $$ = newExpNode(OpK);
                $$->attr.op = MAI;
            } | IMA{
                $$ = newExpNode(OpK);
                $$->attr.op = IMA;
            } | IGL{
                $$ = newExpNode(OpK);
                $$->attr.op = IGL;
            } | DIF{
                $$ = newExpNode(OpK);
                $$->attr.op = DIF;
            };

soma-expressao: soma-expressao soma termo{
                    $$ = newExpNode(CalcK);
                    $$->child[0] = $1;
                    $$->child[1] = $2;
                    $$->child[2] = $3;
                } | termo{
                    $$ = $1;
                };

soma: SOM{
        $$ = newExpNode(OpK);
        $$->attr.op = SOM;
    } | SUB{
        $$ = newExpNode(OpK);
        $$->attr.op = SUB;
    };

termo: termo mult fator{
            $$ = newExpNode(CalcK);
            $$->child[0] = $1;
            $$->child[1] = $2;
            $$->child[2] = $3;
        } | fator{
            $$ = $1;
        };

mult: MUL{
        $$ = newExpNode(OpK);
        $$->attr.op = MUL;
    } | DIV{
        $$ = newExpNode(OpK);
        $$->attr.op = DIV;
    };

fator: APR expressao FPR{
            $$ = $2;
        } | var{
            $$ = $1;
        } | ativacao{
            $$ = $1;
        } | num{
            $$ = newExpNode(ConstK);
            $$->type = Integer;
            $$->attr.val = atoi(tokenString);
        };

ativacao: id{
            $$ = newExpNode(CallK);
            $$->attr.name = savedName;
        } APR args FPR{
            $$ = $2;
            $$->child[0] = $4;
        };

args: arg-lista{
        $$ = $1;
    } | {
        $$ = NULL;
    };

arg-lista: arg-lista VIR expressao{
                YYSTYPE t = $1;
                if (t != NULL) {
                while (t->sibling != NULL) { 
                    t = t->sibling; 
                }
                t->sibling = $3;
                $$ = $1;
                } else {
                $$ = $3;
                }
            } | expressao{
                $$ = $1;
            };

id: ID {
        savedName = copyString(tokenString);
        savedLineNo = lineno;
    };

num: NUM{
        savedNumber = atoi(tokenString);
        savedLineNo = lineno;
    }
%%

int yyerror(char * message){
    fprintf(errorfile, "ERRO SINTÁTICO: ");
    printToken(1, yychar, tokenString);    
    fprintf(errorfile, " LINHA: %d\n", lineno);
    Error = TRUE;
    return 0;
}

static int yylex(void)
{ return getToken(); }

TreeNode * parse(void){
    yyparse();
    return savedTree;
}