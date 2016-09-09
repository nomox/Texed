#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "memory.h"
#include "expression.h"
#include "tokens.h"
#include "error.h"

static Expression *buildExpression(DataType type, void *executer);

static int _get_num(Expression *e) {
  return e->result.i;
}
static char* _get_str(Expression *e) {
  return e->result.s;
}
static bool _get_bol(Expression *e) {
  return e->result.b;
}
static bool _get_nil(Expression *e) {
  return e->result.i;
}
// вирази для пацанів
// variable expression
Expression* variableExpression(char *name) {
  Record r = *memoryGet(name);
  if (r.type == dtNUMBER) {
    return numberExpression(r.data.i);
  }
  if (r.type == dtSTRING) {
    return stringExpression(r.data.s);
  }
  if (r.type == dtBOOLEAN) {
    return boolExpression(r.data.s);
  }
  if (r.type == dtNIL || r.type == dtDELETED) {
    return nilExpression();
  }
}
// basic expression (for all data types)
Expression* numberExpression(int number) {
  Expression *e = buildExpression(dtNUMBER, &_get_num); // new example of number
  e->result.i = number;
  return e;
}
Expression* stringExpression(char *string) {
  char *str = (char*)malloc(strlen(string));
  strcpy(str, string);
  Expression *e = buildExpression(dtSTRING, &_get_str); // new example of string
  e->result.s = string;
  return e;
}
Expression* boolExpression(bool boolean) {
  Expression *e = buildExpression(dtBOOLEAN, &_get_bol); // new example of boolean
  e->result.b = boolean;
  return e;
}
Expression* nilExpression() {
  Expression *e = buildExpression(dtNIL, &_get_nil); // new example of boolean
  e->result.i = 0;
  return e;
}
// operators
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

Expression* conditionalExpression(Operation operation, Expression *expr1, Expression *expr2) {
  switch (operation) {
    case opEQ:
      return boolExpression((int)expr1->get(expr1) == (int)expr2->get(expr2));
    case opGRTH:
      return boolExpression((int)expr1->get(expr1) > (int)expr2->get(expr2));
    case opLSTH:
      return boolExpression((int)expr1->get(expr1) < (int)expr2->get(expr2));
  }
  //return expr1;
}

// builder
static Expression *buildExpression(DataType type, void *executer) {
  Expression *e = (Expression*)malloc(sizeof(Expression)); // new example of boolean
  e->type = type;
  e->get = executer;
  return e;
}
