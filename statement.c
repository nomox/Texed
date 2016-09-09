#include "stdlib.h"
#include "stdbool.h"
#include "statement.h"
#include "memory.h"
#include "error.h"

static Statement *buildStatement();

static void _assign_stat(Statement *s) {
  StatementAssign *st = s->statement;
  // assign controller
  if (st->expression->type == dtNUMBER)
    memorySet(newRecordNumber(st->name, st->expression->get(st->expression)));
  if (st->expression->type == dtSTRING)
    memorySet(newRecordString(st->name, st->expression->get(st->expression)));
  free(st->expression); // звільняєм память щоб нас не сварили
  free(st);
}
static void _condition_stat(Statement *s) {
  StatementCondition *st = s->statement;
  bool res = st->expression->get(st->expression);
  if (res) {
    st->ifSt->execute(st->ifSt);
  }
  else if (st->elseSt != NULL) {
    st->elseSt->execute(st->elseSt);
  }
  free(st->expression);
  free(st);
}
static void _print_stat(Statement *s) {
  StatementPrint *st = s->statement;
  if (st->expression->type == dtNUMBER) // print number
    printf("%d", st->expression->get(st->expression));
  if (st->expression->type == dtSTRING) // print string
    printf(st->expression->get(st->expression));
  if (st->expression->type == dtBOOLEAN) // print boolean
    printf(st->expression->get(st->expression)?"true":"false");
  if (st->expression->type == dtNIL) // print nil
    printf("nil");
  free(st->expression); // звільняєм память щоб нас не сварили
  free(st);
}
static void _error_stat(Statement *s) {
  StatementPrint *st = s->statement;
  if (st->expression->type == dtSTRING)
    printf(st->expression->get(st->expression));
  else
    writeError(erEXPECTATION, "<EXPRESSION>");
  free(st->expression); // звільняєм память щоб нас не сварили
  free(st);
  exit(0);
}
static void _delete_stat(Statement *s) {
  StatementAssign *st = s->statement;
  memoryDelete(st->name);
}

Statement *AssignStatement(char *name, Expression *expr) {
  Statement *s = (Statement*)malloc(sizeof(Statement));
  StatementAssign *st = (StatementAssign*)malloc(sizeof(StatementAssign));
  st->name = name;
  st->expression = expr;
  return buildStatement(stASSIGN, st, &_assign_stat);
}
Statement *ConditionStatement(Expression *expr, Statement *if_st, Statement *el_st) {
  StatementCondition *st = (StatementCondition*)malloc(sizeof(StatementCondition));
  st->expression = expr;
  st->ifSt = if_st;
  st->elseSt = el_st;
  return buildStatement(stCONDITION, st, &_condition_stat);
}
Statement *PrintStatement(Expression *expr) {
  StatementCondition *st = (StatementPrint*)malloc(sizeof(StatementPrint));
  st->expression = expr;
  return buildStatement(stPRINT, st, &_print_stat);
}
Statement *ErrorStatement(Expression *expr) {
  StatementError *st = (StatementError*)malloc(sizeof(StatementError));
  st->expression = expr;
  return buildStatement(stERROR, st, &_error_stat);
}
Statement *DeleteStatement(char *name) {
  StatementDelete *st = (StatementDelete*)malloc(sizeof(StatementDelete));
  st->name = name;
  return buildStatement(stDELETE, st, &_delete_stat);
}

static Statement *buildStatement(StatementType st_type, void *st, void *executer) {
  Statement *s = (Statement*)malloc(sizeof(Statement));
  s->statement = st;
  s->type = st_type;
  s->execute = executer;
  return s;
}
