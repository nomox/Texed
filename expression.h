#pragma once

typedef enum OPERATION {
  opMUL,
  opDIV,
  opADD,
  opSUB,
  opNEGNUM // negative number
} Operation;
typedef enum RESULT_TYPE {
  rtNUMBER,
  rtSTRING
} ResultType;
typedef struct EXPRESSION {
  ResultType type;
  union RESULT {
    float f; // number float
    int i; // number float
    char *s; // string
  } result;
  void* (*get)(struct Expression*);
} Expression;

Expression* numberExpression(int);
Expression* unaryExpression(Operation, Expression*);
Expression* binaryExpression(Operation, Expression*, Expression*);
