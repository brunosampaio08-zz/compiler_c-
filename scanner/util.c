/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case ELSE:
    case IF:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
    fprintf(listing, "reserved word: %s\n",tokenString);
    break;
    case SOM:     fprintf(listing, "+\n");  break;
    case SUB:    fprintf(listing, "-\n");  break;
    case MUL:    fprintf(listing, "*\n");  break;
    case DIV:     fprintf(listing, "/\n");  break;
    case MEN:       fprintf(listing, "<\n");  break;
    case IME:       fprintf(listing, "<=\n"); break;
    case MAI:       fprintf(listing, ">\n");  break;
    case IMA:       fprintf(listing, ">=\n"); break;
    case IGL:       fprintf(listing, "==\n"); break;
    case DIF:       fprintf(listing, "!=\n"); break;
    case ATR:   fprintf(listing, "=\n");  break;
    case PEV:     fprintf(listing, ";\n");  break;
    case VIR:    fprintf(listing, ",\n");  break;
    case APR:   fprintf(listing, "(\n");  break;
    case FPR:   fprintf(listing, ")\n");  break;
    case ACL: fprintf(listing, "[\n");  break;
    case FCL: fprintf(listing, "]\n");  break;
    case ACH:   fprintf(listing, "{\n");  break;
    case FCH:   fprintf(listing, "}\n");  break;
    case ENDFILE:  fprintf(listing,"%s %s\n",  "ENDFILE", "EOF"); break;
    case NUM: fprintf(listing, "NUM, val = %s\n",tokenString); break;
    case ID: fprintf(listing, "ID, name = %s\n",tokenString); break;
    case ERR: fprintf(listing, "ERROR: %s\n",tokenString); break;
    default: /* should never happen */
       fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function newDeclNode creates a new declaration
 * node for syntax tree construction
 */
TreeNode * newDeclNode(DeclKind kind) 
{
  TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DeclK;
    t->kind.decl = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* printType print types of funcions and variables */
void printTypes(TreeNode* tree) {
  if (tree->child[0] != NULL) {
    switch (tree->child[0]->type) {
      case Integer:
        fprintf(listing,"int");
        break;
      case Void:
        fprintf(listing,"void");
        break;
      case IntegerArray:
        fprintf(listing,"int array");
        break;
      default: return;
    }
  } else {
    switch (tree->type) {
      case Integer:
        fprintf(listing,"int");
        break;
      case Void:
        fprintf(listing,"void");
        break;
      case IntegerArray:
        fprintf(listing,"int array");
        break;
      default: return;
    }
  }
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  // IfK, WhileK, CompoundK, ReturnK
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK) {
      printSpaces();
      switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"If\n");
          break;
        case WhileK:
          fprintf(listing, "While\n");
          break;
        case CompoundK:
          fprintf(listing, "Compound statement\n");
          break;
        case ReturnK:
          fprintf(listing, "Return\n");
          break;
        case AssignK:
          fprintf(listing, "Assign\n");
          break;
        default:
          fprintf(listing, "Unknown stmtNode kind\n");
          break;
        }
    }
    // OpK, ConstK, AssignK, IdK, TypeK, ArrIdK, CallK, CalK
    else if (tree->nodekind==ExpK)
    { if (tree->kind.exp != TypeK) printSpaces();
      switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: ");
          printToken(tree->attr.op, "\0");
          break;
        case ConstK:
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"Id: %s\n",tree->attr.name);
          break;
        case TypeK: break;
        case ArrIdK:
          fprintf(listing,"ArrId \n");
          break;
        case CallK:
          fprintf(listing, "Call Function : %s\n", tree->attr.name);
          break;
        case CalcK:
          fprintf(listing, "Operator : ");
          printToken(tree->child[1]->attr.op, "\0");
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
  }
  // VarK, FunK, ArrVarK, ArrParamK, ParamK
    else if (tree->nodekind==DeclK) {
      printSpaces();
      switch (tree->kind.decl) {
        case FunK :
          fprintf(listing, "Function Declaration:  ");
          printTypes(tree);
          fprintf(listing, " %s()\n", tree->attr.name);
          break;
        case VarK :
          fprintf(listing, "Variable Declaration:  ");
          printTypes(tree);
          fprintf(listing, " %s;\n", tree->attr.name);
          break;
        case ArrVarK :
          fprintf(listing, "Array Variable Declaration:  ");
          printTypes(tree);
          fprintf(listing," %s[%d];\n", tree->attr.arr.name, tree->attr.arr.size);
          break;
        case ArrParamK :
          fprintf(listing, "Array Parameter: %s\n", tree->attr.name);
          break;
        case ParamK :
          fprintf(listing, "Parameter: ");
          printTypes(tree);
          if (tree->attr.name != NULL) {
            fprintf(listing, " %s\n", tree->attr.name);
          } else {
            fprintf(listing, " void\n");
          }
          break;
        default:
          fprintf(listing, "Unknown Declaration\n");
          break;
        }
      }else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++) 
      if (tree->child[i] != NULL) 
        printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}
