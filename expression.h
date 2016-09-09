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
typedef struct EXPRESSION {
  DataType type;
  Data result;
  void* (*get)(struct Expression*);
} Expression;

Expression* variableExpression(char*);
Expression* numberExpression(int);
Expression* stringExpression(char*);
Expression* boolExpression(bool);
Expression* nilExpression();
Expression* unaryExpression(Operation, Expression*);
Expression* binaryExpression(Operation, Expression*, Expression*);
Expression* conditionalExpression(Operation, Expression*, Expression*);
