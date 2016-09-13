#pragma once

#include "memory.h"

typedef enum OPERATION {
  opMUL,
  opDIV,
  opADD,
  opSUB,
  opNEGNUM, // negative number
  opGRTH,
  opLSTH,
  opEQ
} Operation;

typedef struct expression_value {
  DataType type;
  Data value;
} expression_value_t;

typedef struct EXPRESSION {
  void *expression;
  void* (*get)(struct Expression*);
} Expression;
// expressions
typedef struct EXPRESSION_VARIABLE {
  char *name;
} ExpressionVariable;
typedef struct EXPRESSION_INTEGER {
  int number;
} ExpressionInteger;
typedef struct EXPRESSION_FLOAT {
  float number;
} ExpressionFloat;
typedef struct EXPRESSION_STRING {
  char *string;
} ExpressionString;
typedef struct EXPRESSION_BOOLEAN {
  bool boolean;
} ExpressionBoolean;
typedef struct EXPRESSION_NIL {

} ExpressionNil;
typedef struct EXPRESSION_UNARY {
  Operation operation;
  Expression *expr;
} ExpressionUnary;
typedef struct EXPRESSION_BINARY {
  Operation operation;
  Expression *expr1;
  Expression *expr2;
} ExpressionBinary;
typedef struct EXPRESSION_CONDITIONAL {
  Operation operation;
  Expression *expr1;
  Expression *expr2;
} ExpressionConditional;

Expression* variableExpression(char*);
Expression* integerExpression(int);
Expression* floatExpression(float);
Expression* stringExpression(char*);
Expression* boolExpression(bool);
Expression* nilExpression();
Expression* unaryExpression(Operation, Expression*);
Expression* binaryExpression(Operation, Expression*, Expression*);
Expression* conditionalExpression(Operation, Expression*, Expression*);
expression_value_t *getValueExpression(Expression*);
