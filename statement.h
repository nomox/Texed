#ifndef STATEMENT_H
#define STATEMENT_H

#include "expression.h"
#include "setjmp.h"

typedef enum STATEMENTTYPE {
  stASSIGN,
  stCONDITION,
  stWHILE,
  stFOR,
  stPRINT,
  stERROR,
  stDELETE,
  stBLOCK,
  stBREAK,
  stCONTINUE,
  stFUNCDEF,
  stRETURN,
  stISTRUE,
  stISFALSE,
  stDEFAULT,
  stTABLE,
  stCLOSETABLE
} StatementType;

typedef struct STATEMENT {
  StatementType type;
  void* statement;
  void* (*execute)(struct STATEMENT*);
} Statement;
// statements
typedef struct STATEMENT_ASSIGN {
  char *name;
  Expression *expression;
} StatementAssign;
typedef struct STATEMENT_CONDITION {
  Expression *expression;
  Statement *ifSt;
  Statement *elseSt;
} StatementCondition;
typedef struct STATEMENT_WHILE {
  Expression *expression;
  Statement *whileSt;
} StatementWhile;
typedef struct STATEMENT_FOR {
  char *name;
  Expression *expr;
  Statement *forSt;
} StatementFor;
//opearators
typedef struct STATEMENT_PRINT {
  Expression *expression;
} StatementPrint;
typedef struct STATEMENT_ERROR {
  Expression *expression;
} StatementError;
typedef struct STATEMENT_DELETE {
  char *name;
} StatementDelete;
typedef struct STATEMENT_BLOCK {
  struct statement_node *list;
} StatementBlock;
typedef struct STATEMENT_FUNCDEF {
  char *name;
  char *args;
  Statement *func_body;
} StatementFuncDef;
typedef struct STATEMENT_RETURN {
  Expression *expression;
} StatementReturn;
typedef struct STATEMENT_ISTRUE {
  Expression *expression;
  Statement *statement;
} StatementIsTrue;
typedef struct STATEMENT_ISFALSE {
  Expression *expression;
  Statement *statement;
} StatementIsFalse;
typedef struct STATEMENT_DEFAULT {
  Expression *expression;
} StatementDefault;
typedef struct STATEMENT_TABLE {
  char *name;
} StatementTable;
typedef struct STATEMENT_ARRAYASSIGN {
  char *name; // array variable
  expression_node_t *indices; // index
  Expression *expression; // assignment value
} StatementArrayAssign;

Statement *AssignStatement(char*, Expression*);
Statement *ConditionStatement(Expression*, Statement*, Statement*);
Statement *WhileStatement(Expression*, Statement*);
Statement *ForStatement(char*, Expression*, Statement*);
Statement *PrintStatement(Expression*);
Statement *ErrorStatement(Expression*);
Statement *DeleteStatement(char*);
Statement *BlockStatement();
Statement *BreakStatement();
Statement *ContinueStatement();
Statement *FuncDefStatement(char*, char*, Statement*);
Statement *ReturnStatement(Expression*);
Statement *IfTrueStatement(Expression*, Statement*);
Statement *IfFalseStatement(Expression*, Statement*);
Statement *DefaultStatement(Expression*);
Statement *TableStatement(char*);
Statement *CloseTableStatement();
Statement *ArrayAssignStatement(char*, expression_node_t*, Expression*);

// return utils
jmp_buf jump_return;
expression_value_t *returned_val;

// list
typedef struct statements_node {
  struct STATEMENT *value;
  struct statements_node *next;
} statement_node_t;

statement_node_t *statements_init();
void statements_push(statement_node_t*, Statement*);

#endif STATEMENT_H
