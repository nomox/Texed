#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "error.h"
#include "expression.h"
#include "statement.h"
#include "memory.h"
#include "function.h"

static Expression *buildExpression(void *expr, void *executer); // build expression

static expression_value_t *_get_int(Expression *e) {
  ExpressionInteger *ex = e->expression;
  return valueInteger(ex->number);
}
static expression_value_t *_get_float(Expression *e) {
  ExpressionFloat *ex = e->expression;
  return valueFloat(ex->number);
}
static expression_value_t *_get_str(Expression *e) {
  ExpressionString *ex = e->expression;
  return valueString(ex->string);
}
static expression_value_t *_get_bool(Expression *e) {
  ExpressionBoolean *ex = e->expression;
  return valueBoolean(ex->boolean);
}
static expression_value_t *_get_nil(Expression *e) { // fix: bool to int
  return valueNil();
}
// variable
static expression_value_t *_get_var(Expression *e) { // fix: bool to int
  ExpressionVariable *ex = e->expression;
  Record r = *memoryGet(ex->name);
  if (r.type == dtINTEGER) {
    return valueInteger(r.data.i);
  }
  if (r.type == dtFLOAT) {
    return valueFloat(r.data.f);
  }
  if (r.type == dtSTRING) {
    return valueString(r.data.s);
  }
  if (r.type == dtBOOLEAN) {
    return valueBoolean(r.data.b);
  }
  if (r.type == dtNIL) {
    return valueNil();
  }
  if (r.type == dtTABLE) {
    return valueTable(r.data.table);
  }
}
static expression_value_t *_get_suffix(Expression *e) {
  ExpressionSuffix *ex = e->expression;
  expression_value_t *table = getValueExpression(ex->expr1);
  if (table->type != dtTABLE) // constructor
    writeError(erEXPECTATION, "<TABLE>");
  current_handler = table->value.table;
  expression_value_t *res = getValueExpression(ex->expr2);
  if (restore_handler == NULL)
    writeError(erNULL, "restore handler is NULL");
  current_handler = restore_handler;
  return res;
}
// operators
static expression_value_t *_get_unary(Expression *e) {
  ExpressionUnary *ex = e->expression;
  expression_value_t *eval = getValueExpression(ex->expr);
  if (ex->operation == opNEGNUM)
    return valueInteger(-(eval->value.i));
}
static expression_value_t *_get_binary(Expression *e) {
  ExpressionBinary *ex = e->expression;
  expression_value_t *eval1 = getValueExpression(ex->expr1);
  expression_value_t *eval2 = getValueExpression(ex->expr2);
  // contoller
  switch (ex->operation) {
  case opSUB:
    return valueInteger(eval1->value.i - eval2->value.i);
  case opMUL:
    return valueInteger(eval1->value.i * eval2->value.i);
  case opDIV:
    if (eval2->value.f == 0) {
          writeError(erZERODIVISION, NULL);
    }
    return valueInteger(eval1->value.i / eval2->value.i);
  case opADD:
    return valueInteger(eval1->value.i + eval2->value.i);
  }
}
static expression_value_t *_get_conditional(Expression *e) {
  ExpressionConditional *ex = e->expression;
  expression_value_t *eval1 = getValueExpression(ex->expr1);
  expression_value_t *eval2 = getValueExpression(ex->expr2);

  switch (ex->operation) {
    case opEQ:
      return valueBoolean(eval1->value.i == eval2->value.i);
    case opGRTH:
      return valueBoolean(eval1->value.i > eval2->value.i);
    case opLSTH:
      return valueBoolean(eval1->value.i < eval2->value.i);
  }
}
// function
static expression_value_t *_get_function(Expression *e) {
  ExpressionFunction *ex = e->expression;
  int arg_len = expression_length(ex->args);
  expression_value_t **args = malloc(arg_len); // create args array
  Expression *tmp;
  expression_node_t *current;
  int i;
  for (i = 0, current = ex->args; current != NULL; i++, current = current->next) {
    tmp = current->value;
    args[i] = tmp->get(tmp);
  }
  return callFunction(ex->name, args, arg_len);
}

// вирази для пацанів
// variable expression
Expression* variableExpression(char *name) {
  ExpressionVariable *e = (ExpressionVariable*)malloc(sizeof(ExpressionVariable));
  e->name = malloc(strlen(name));
  strcpy(e->name, name);
  return buildExpression(e, &_get_var);
}
// basic expression (for all data types)
Expression* integerExpression(int number) {
  ExpressionInteger *e = (ExpressionInteger*)malloc(sizeof(ExpressionInteger));
  e->number = number;
  return buildExpression(e, &_get_int);
}
Expression* floatExpression(double number) {
  ExpressionFloat *e = (ExpressionFloat*)malloc(sizeof(ExpressionFloat));
  e->number = number;
  return buildExpression(e, &_get_float);
}
Expression* stringExpression(char *string) {
  ExpressionString *e = (ExpressionString*)malloc(sizeof(ExpressionString));
  e->string = (char*)malloc(strlen(string));
  strcpy(e->string, string);
  return buildExpression(e, &_get_str);
}
Expression* boolExpression(bool boolean) {
  ExpressionBoolean *e = (ExpressionBoolean*)malloc(sizeof(ExpressionBoolean));
  e->boolean = boolean;
  return buildExpression(e, &_get_bool);
}
Expression* nilExpression() {
  ExpressionNil *e = (ExpressionNil*)malloc(sizeof(ExpressionNil));
  return buildExpression(e, &_get_nil);
}
// operators
Expression* unaryExpression(Operation operation, Expression *expr) {
  ExpressionUnary *e = (ExpressionUnary*)malloc(sizeof(ExpressionUnary));
  e->operation = operation;
  e->expr = expr;
  return buildExpression(e, &_get_unary);
}
Expression* binaryExpression(Operation operation, Expression *expr1, Expression *expr2) {
  ExpressionBinary *e = (ExpressionBinary*)malloc(sizeof(ExpressionBinary));
  e->operation = operation;
  e->expr1 = expr1;
  e->expr2 = expr2;
  return buildExpression(e, &_get_binary);
}

Expression* conditionalExpression(Operation operation, Expression *expr1, Expression *expr2) {
  ExpressionConditional *e = (ExpressionConditional*)malloc(sizeof(ExpressionConditional));
  e->operation = operation;
  e->expr1 = expr1;
  e->expr2 = expr2;
  return buildExpression(e, &_get_conditional);
}
Expression* functionExpression(char *name, expression_node_t *args) {
  ExpressionFunction *e = (ExpressionFunction*)malloc(sizeof(ExpressionFunction));
  e->name = malloc(strlen(name));
  strcpy(e->name, name);
  e->args = args;
  return buildExpression(e, &_get_function);
}
Expression* suffixExpression(Expression *expr1, Expression *expr2) {
  ExpressionSuffix *e = (ExpressionSuffix*)malloc(sizeof(ExpressionSuffix));
  e->expr1 = expr1;
  e->expr2 = expr2;
  return buildExpression(e, &_get_suffix);
}

// builder
static Expression *buildExpression(void *expr, void *executer) {
  Expression *e = (Expression*)malloc(sizeof(Expression)); // new example of boolean
  e->expression = expr;
  e->get = executer;
  return e;
}

// tools
expression_value_t *valueInteger(int number) {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtINTEGER;
  ev->value.i = number;
  return ev;
}
expression_value_t *valueFloat(float number) {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtFLOAT;
  ev->value.f = number;
  return ev;
}
expression_value_t *valueBoolean(bool boolean) {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtBOOLEAN;
  ev->value.b = boolean;
  return ev;
}
expression_value_t *valueString(char *string) {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtSTRING;
  ev->value.s = malloc(strlen(string));
  strcpy(ev->value.s, string);
  return ev;
}
expression_value_t *valueNil() {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtNIL;
  return ev;
}
expression_value_t *valueTable(memory_node_t *table) {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtTABLE;
  ev->value.table = table;
  return ev;
}
expression_value_t *getValueExpression(Expression *e) {
  return e->get(e);
}
void storeCurrentHandler() {
  restore_handler = current_handler;
}
// tools
expression_node_t *expression_init() {
  expression_node_t *node = (expression_node_t*)malloc(sizeof(expression_node_t));
	node->value = NULL;
	node->next = NULL;
	return node;
}
void expression_push(expression_node_t *node, Expression *expr){
  statement_node_t *current = node;

  if (current->value == NULL) { // check for first element
    current->value = expr;
    return;
  }

  while (current->next != NULL) {
      current = current->next;
  }

  current->next = (expression_node_t*)malloc(sizeof(expression_node_t));
  current->next->value = expr;
  current->next->next = NULL;
}
int expression_length(expression_node_t *list) {
  int length = 0;
  expression_node_t *current;
  for (current = list; current != NULL; current = current->next)
    length++;
  return length;
}
