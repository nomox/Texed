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
  ExpressionNumber *ex = e->expression;
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
  PreparedVariable var = prepareVariable(ex->name);
  Record *r = memoryGet(var.parent_handler, var.name);
  if (r->type == dtINTEGER) {
    return valueInteger(r->data.i);
  }
  if (r->type == dtFLOAT) {
    return valueFloat(r->data.f);
  }
  if (r->type == dtSTRING) {
    return valueString(r->data.s);
  }
  if (r->type == dtBOOLEAN) {
    return valueBoolean(r->data.b);
  }
  if (r->type == dtNIL) {
    return valueNil();
  }
  if (r->type == dtTABLE) {
    return valueTable(r->data.table);
  }
  if (r->type == dtLIST) {
    return valueList(r->data.list);
  }
  if (r->type == dtFUNCTION) {
    return valueFunction(r->data.fn);
  }
  return NULL;
}
// operators
static expression_value_t *_get_unary(Expression *e) {
  ExpressionUnary *ex = e->expression;
  expression_value_t *eval = getValueExpression(ex->expr);
  if (ex->operation == opNEGNUM)
    return valueInteger(-(eval->value.i));
  return NULL;
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
  default:
    return NULL;
  }
}
static expression_value_t *_get_conditional(Expression *e) {
  ExpressionConditional *ex = e->expression;
  expression_value_t *eval1 = getValueExpression(ex->expr1);
  expression_value_t *eval2 = getValueExpression(ex->expr2);

  switch (ex->operation) {
    case opEQ:
      return valueBoolean(eval1->value.i == eval2->value.i);
    case opNEQ:
      return valueBoolean(eval1->value.i != eval2->value.i);
    case opGRTH:
      return valueBoolean(eval1->value.i > eval2->value.i);
    case opLSTH:
      return valueBoolean(eval1->value.i < eval2->value.i);
    case opGREQ:
      return valueBoolean(eval1->value.i >= eval2->value.i);
    case opLSEQ:
      return valueBoolean(eval1->value.i <= eval2->value.i);
    default:
      return NULL;
  }
}
// function
static expression_value_t *_get_function(Expression *e) {
  ExpressionFunction *ex = e->expression;
  PreparedVariable var = prepareVariable(ex->name);
  int arg_len = expression_length(ex->args);
  expression_value_t **args = malloc(arg_len); // create args array
  Expression *tmp;
  expression_node_t *current;
  int i;
  for (i = 0, current = ex->args; current != NULL; i++, current = current->next) {
    tmp = current->value;
    args[i] = tmp->get(tmp);
  }
  return callFunction(var, args, arg_len);
}
static expression_value_t *_get_list(Expression *e) {
  ExpressionList *ex = e->expression;
  list_node_t *list = NULL;
  Expression *tmp;
  expression_node_t *current;
  for (current = ex->elements; current != NULL; current = current->next) {
    tmp = current->value;
    list_push(&list, getValueExpression(tmp));
  }
  return valueList(list);
}
static expression_value_t *_get_element(Expression *e) {
  ExpressionElement *ex = e->expression;
  PreparedVariable var = prepareVariable(ex->name);
  Record *r = memoryGet(var.parent_handler, var.name); // get list
  if (r->type != dtLIST) // check for list
    writeError(erNOTANARRAY, var.name);
  list_node_t *list = r->data.list;

  expression_value_t *idxValue = NULL; // current index
  expression_value_t *elValue = NULL; // elemnt value
  expression_node_t *current; // current expression
  int i;
  for (i = 0, current = ex->indices; current != NULL; i++, current = current->next) {
    if (i > 0) { // skip first iteration
      if (elValue->type != dtLIST) // check for list
        writeError(erNOTANARRAY, var.name);
      list = elValue->value.list;
    }
    idxValue = getValueExpression(current->value); // get index
    if (idxValue->type != dtINTEGER) // index is value of integer
      writeError(erEXPECTATION, "<Index> (integer)");
    elValue = list_get(list, (unsigned int)idxValue->value.list);
    if (elValue == NULL)
      writeError(erOUTOFRANGE, var.name);
  }
  return elValue;
}

// вирази для пацанів
// variable expression
Expression* variableExpression(char *name) {
  ExpressionVariable *e = (ExpressionVariable*)malloc(sizeof(ExpressionVariable));
  e->name = name;
  return buildExpression(e, &_get_var);
}
// basic expression (for all data types)
Expression* integerExpression(int number) {
  ExpressionInteger *e = (ExpressionInteger*)malloc(sizeof(ExpressionInteger));
  e->number = number;
  return buildExpression(e, &_get_int);
}
Expression* floatExpression(double number) {
  ExpressionNumber *e = (ExpressionNumber*)malloc(sizeof(ExpressionNumber));
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
  e->name = name;
  e->args = args;
  return buildExpression(e, &_get_function);
}
Expression* listExpression(expression_node_t *elements) {
  ExpressionList *e = (ExpressionList*)malloc(sizeof(ExpressionList));
  e->elements = elements;
  return buildExpression(e, &_get_list);
}
Expression* elementExpression(char *name, expression_node_t *indices) {
  ExpressionElement *e = (ExpressionElement*)malloc(sizeof(ExpressionElement));
  e->name = name;
  e->indices = indices;
  return buildExpression(e, &_get_element);
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
expression_value_t *valueList(list_node_t *list) {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtLIST;
  ev->value.list = list;
  return ev;
}
expression_value_t *valueFunction(Function *function) {
  expression_value_t *ev = (expression_value_t*)malloc(sizeof(expression_value_t));
  ev->type = dtFUNCTION;
  ev->value.fn = function;
  return ev;
}
expression_value_t *getValueExpression(Expression *e) {
  return e->get(e);
}

// tools
void expression_push(expression_node_t **node, Expression *expr){
  if (*node == NULL) {
    *node = malloc(sizeof(expression_node_t));
    (*node)->value = expr;
    (*node)->next = NULL;
    return;
  }
  expression_node_t *current = *node;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = malloc(sizeof(expression_node_t));
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
