#include "stdlib.h"
#include "expression.h"
#include "tokens.h"
#include "error.h"

int _get_num(Expression *e) {
  return e->result.i;
}
Expression* numberExpression(int number) {
  Expression *e = (Expression*)malloc(sizeof(Expression)); // new example of number
  e->type = rtNUMBER;
  e->result.i = number;
  e->get = &_get_num;
  return e;
}
Expression* unaryExpression(Operation operation, Expression *expr) {
  // єслі шо то мож почистити вирази з памяті free(expr)
  if (operation == opNEGNUM)
    return numberExpression(-((int)expr->get(expr)));
  return expr;
}
Expression* binaryExpression(Operation operation, Expression *expr1, Expression *expr2) {
  switch (operation) {
    case opSUB:
      return numberExpression((int)expr1->get(expr1) - (int)expr2->get(expr2));
    case opMUL:
      return numberExpression((int)expr1->get(expr1) * (int)expr2->get(expr2));
    case opDIV:
      if ((int)expr2->get(expr2) == 0) {
        writeError(erZERODIVISION, NULL);
      }
      return numberExpression((int)expr1->get(expr1) / (int)expr2->get(expr2));
    case opADD:
    default:
      return numberExpression((int)expr1->get(expr1) + (int)expr2->get(expr2));
  }
  //return expr1;
}
