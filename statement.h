#pragma once

#include "expression.h"

typedef enum STATEMENTTYPE {
  stASSIGN,
  stCONDITION,
  stPRINT,
  stERROR,
  stDELETE
} StatementType;

typedef struct STATEMENT {
  StatementType type;
  void* statement;
  void* (*execute)(struct Statement*);
} Statement;
typedef struct STATEMENT_ASSIGN {
  char *name;
  Expression *expression;
} StatementAssign;
typedef struct STATEMENT_CONDITION {
  Expression *expression;
  Statement *ifSt;
  Statement *elseSt;
} StatementCondition;
typedef struct STATEMENT_PRINT {
  Expression *expression;
} StatementPrint;
typedef struct STATEMENT_ERROR {
  Expression *expression;
} StatementError;
typedef struct STATEMENT_DELETE {
  char *name;
} StatementDelete;

Statement *AssignStatement(char*, Expression*);
Statement *ConditionStatement(Expression*, Statement*, Statement*);
Statement *PrintStatement(Expression*);
Statement *ErrorStatement(Expression*);
Statement *DeleteStatement(char*);
