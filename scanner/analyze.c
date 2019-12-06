/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "analyze.h"
#include "util.h"
#include "symboltable.h"

FILE * errorfile;

/*
 * To insert to symbol table build-in functions
 * int input()   // One integer value is input from the user.
 * void output() // Prints the value of arg.
 */

static void insertInputFunc(void) {
  TreeNode * fun_declaration = newDeclNode(FunK);
  fun_declaration->type = Integer;

  TreeNode * type_specifier = newExpNode(TypeK);
  type_specifier->attr.type = INT;

  TreeNode * compound_stmt = newStmtNode(CompoundK);
  compound_stmt->child[0] = NULL;
  compound_stmt->child[1] = NULL;

  fun_declaration->lineno = 0;
  fun_declaration->attr.name = "input";
  fun_declaration->child[0] = type_specifier;
  fun_declaration->child[1] = NULL;
  fun_declaration->child[2] = compound_stmt;

  /* Insert input function*/
  st_insert("global", "input", Integer, fun_declaration, 1);
}

/* To insert to symbol table build-in functions
 * int input()   // One integer value is input from the user.
 * void output() // Prints the value of arg.
 */

static void insertOutputFunc(void) {

  TreeNode * fun_declaration = newDeclNode(FunK);
  fun_declaration->type = Void;

  TreeNode * type_specifier = newDeclNode(FunK);
  type_specifier->attr.type = VOID;

  TreeNode * params = newDeclNode(ParamK);
  params->attr.name = "arg";
  params->child[0] = newExpNode(TypeK);
  params->child[0]->attr.type = INT;

  TreeNode * compound_stmt = newStmtNode(CompoundK);
  compound_stmt->child[0] = NULL;
  compound_stmt->child[1] = NULL;

  fun_declaration->lineno = 0;
  fun_declaration->attr.name = "output";
  fun_declaration->child[0] = type_specifier;
  fun_declaration->child[1] = params;
  fun_declaration->child[2] = compound_stmt;

  /* Insert output function*/
  st_insert("global", "output", Void, fun_declaration, 0);
}


/* counter for variable memory locations */
static int location = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t, void (* preProc) (TreeNode *), void (* postProc) (TreeNode *) ){ 
  if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

static void popAfterInsertProc(TreeNode * t) {

  if (t->nodekind == StmtK) {
    if (t->kind.stmt == CompoundK) {
      popScope();
    }
  }

  if (t==NULL) return;
  else return;
}

static void symbolError(TreeNode * t, char * message) {
  fprintf(errorfile,"ERRO SEMANTICO: Linha: %d: %s\n", t->lineno, message);
  Error = TRUE;
  //exit(-1);
}

// this is needed to check parameters
static int isFirstCompoundK = 0;
static int locationCounter = 0;

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */

static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { 
    //Statement case
    case StmtK:{
      switch (t->kind.stmt) {
        case CompoundK: {
          if (!isFirstCompoundK) {
            Scope scope = newScope(currScope()->name);
            scope->parent = currScope();
            pushScope(scope);
          }
          isFirstCompoundK = 0;
          break;
        }
        default:
          break;
      }
      break;
    }
    //Expression case
    case ExpK: {
      switch (t->kind.exp){
        case IdK:
        case ArrIdK:
        case CallK: {
          // check undeclation
          if (st_lookup_all_scope(t->attr.name) == NULL){
            symbolError(t, "Simbolo nao definido");
          } else {
            BucketList list = st_lookup_all_scope(t->attr.name);
            t->type = list->type;
            insertLines(t->attr.name, t->lineno);
          }
        }
        default:
          break;
      }
      break;
    }
    
    //Declaration case
    case DeclK: {
      switch (t->kind.decl) {
        case FunK: {
          // initialize location counter
          locationCounter = 0;
          /* Look up scope list to check scope existence */
          if (st_lookup_scope(t->attr.name) != NULL) {
            symbolError(t, "Redefinicao de funcao");
            break;
          }

          if (strcmp(currScope()->name, "global") == 0) {
            st_insert(currScope()->name, t->attr.name, t->child[0]->type, t, locationCounter++);
          }

          Scope scope = newScope(t->attr.name);
          scope->parent = currScope();
          pushScope(scope);
          isFirstCompoundK = 1;
          break;
        }

        //Variable case
        case VarK: {
          /* Look up to check variable existence */
          if (st_lookup(t->attr.name) != NULL) {
            symbolError(t, "Redefinicao de variavel");
            break;
          }

          if(st_lookup_scope(t->attr.name) != NULL){
            symbolError(t, "Declaracao Invalida");
            break;
          }

          // Type Checking : Type should not be void
          if (t->child[0]->type == Void) {
            symbolError(t, "Variavel nao deveria ser do tipo VOID");
            break;
          }

          st_insert(currScope()->name, t->attr.name, t->child[0]->type, t, locationCounter++);
          break;
        }

        //Array case
        case ArrVarK: {

          // Type Checking : Type should not be void
          if (t->child[0]->type == Void) {
            symbolError(t, "Tipo invalido");
            break;
          }

          if(st_lookup_scope(t->attr.arr.name) != NULL){
            symbolError(t, "Declaracao invalida");
            break;
          }

          /*  Look up to check array variable existence  */
          if (st_lookup(t->attr.arr.name) != NULL) {
            symbolError(t, "Vetor ja foi declarado");
            break;
          }
          st_insert(currScope()->name, t->attr.arr.name, t->type, t, locationCounter++);
          break;
        }

        //ArrayParamenter case
        case ArrParamK: {

          // Type Checking : Type should not be void
          if (t->child[0]->type == Void) {
            symbolError(t, "Tipo invalido");
            break;
          }

          if(st_lookup_scope(t->attr.name) != NULL){
            symbolError(t, "Declaracao invalida");
            break;
          }

          /*  Look up to check array parameter existence  */
          if (st_lookup(t->attr.name) != NULL) {
            symbolError(t, "Redefinicao de um parametro vetor");
            break;
          }

          st_insert(currScope()->name, t->attr.name, t->type, t, locationCounter++);
          break;
        }

        //Parameter case
        case ParamK: {

          if (t->attr.name != NULL) {
            /*  Look up to check parameter existence  */
            if(t->child[0]->type == Void){
              symbolError(t, "Tipo invalido");
              break;
            }
            
            if(st_lookup_scope(t->attr.name) != NULL){
              symbolError(t, "Declaracao invalida");
              break;
            }
            
            if (st_lookup(t->attr.name) != NULL) {
              symbolError(t, "Redefinicao de parametro");
              break;
            }

            st_insert(currScope()->name, t->attr.name, t->child[0]->type, t, locationCounter++);
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
   }
  
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ 
  globalScope = newScope("global");
  // push global scope
  pushScope(globalScope);

  insertInputFunc();
  insertOutputFunc();

  traverse(syntaxTree, insertNode, popAfterInsertProc);
  popScope();

  if(st_lookup_scope("main") == NULL){
    fprintf(errorfile, "ERRO SEMANTICO: Main nao declarada!\n");
  }

  if (TraceAnalyze) 
  {
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(errorfile,"Erro de tipo na linha %d: %s\n",t->lineno,message);
  Error = TRUE;
  //exit(-1);
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ 
  switch (t->nodekind)
  {
    case StmtK:
      switch (t->kind.stmt)
      { 
        case AssignK: {
          // Verify the type match of two operands when assigning.
          if (t->child[0]->type == IntegerArray) {
            if(t->child[0]->child[0] == NULL){
              typeError(t->child[0], "Atribuicao para uma variavel do tipo vetor");
            }
          }
          if (t->child[0]->attr.arr.type == Void) {
            typeError(t->child[0], "Atribuicao para uma variavel do tipo VOID");
          }
          if(t->child[1]->kind.exp == CallK){
            if(st_lookup_scope(t->child[1]->attr.name) != NULL){
              if(t->child[1]->type == Void){
                typeError(t->child[1], "Atribuicao de VOID");
              }
            }
          }

          break;
        }
        case ReturnK: {
          const TreeNode * funcDecl;
          const ExpType funcType = funcDecl->type;
          const TreeNode * expr = t->child[0];

          if (funcType == Void && (expr != NULL && expr->type != Void)) {
              typeError(t,"Retorno vazio era esperado");
            } else if (funcType == Integer && (expr == NULL || expr->type == Void)) {
              typeError(t,"Retorno esperado");
            }
        }
        default:
          break;
        }
        break;
      default:
        break;
      }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
