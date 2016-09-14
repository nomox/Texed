#pragma once

#include "expression.h"

typedef enum STATEMENTTYPE {
  stASSIGN,
  stCONDITION,
  stWHILE,
  stPRINT,
  stERROR,
  stDELETE,
  stBLOCK,
  stBREAK,
  stCONTINUE
} StatementType;

typedef struct STATEMENT {
  StatementType type;
  void* statement;
  void* (*execute)(struct Statement*);
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

Statement *AssignStatement(char*, Expression*);
Statement *ConditionStatement(Expression*, Statement*, Statement*);
Statement *WhileStatement(Expression*, Statement*);
Statement *PrintStatement(Expression*);
Statement *ErrorStatement(Expression*);
Statement *DeleteStatement(char*);
Statement *BlockStatement();
Statement *BreakStatement();
Statement *ContinueStatement();

// list
typedef struct statements_node {
  struct STATEMENT *value;
  struct statements_node *next;
} statement_node_t;

statement_node_t *statements_init();
void statements_push(statement_node_t*, Statement*);
