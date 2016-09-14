#include "stdlib.h"
#include "stdbool.h"
#include "setjmp.h"
#include "statement.h"
#include "memory.h"
#include "expression.h"
#include "error.h"

static Statement *buildStatement();

static jmp_buf jump_buffer; // 1 - break, 2 - continue
bool loop_begin = false;

static void _assign_stat(Statement *s) {
  StatementAssign *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
  // assign controller
  if (eval->type == dtINTEGER)
    memorySet(newRecordInteger(st->name, eval->value.i));
  if (eval->type == dtFLOAT)
    memorySet(newRecordFloat(st->name, eval->value.f));
  if (eval->type == dtSTRING)
    memorySet(newRecordString(st->name, eval->value.s));
  if (eval->type == dtBOOLEAN)
    memorySet(newRecordBoolean(st->name, eval->value.b));
  if (eval->type == dtNIL)
    memorySet(newRecordNil(st->name));
  //free(st->expression); // звільняєм память щоб нас не сварили
  //free(st);
}
static void _condition_stat(Statement *s) {
  StatementCondition *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
  if (eval->type != dtBOOLEAN)
    writeError(erEXPECTATION, "<BOOLEAN EXPRESSION>");
  bool res = eval->value.b;
  if (res) {
    st->ifSt->execute(st->ifSt);
  }
  else if (st->elseSt != NULL) {
    st->elseSt->execute(st->elseSt);
  }
  //free(st->expression);
  //free(st);
}
static void _while_stat(Statement *s) {
  StatementWhile *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
  if (eval->type != dtBOOLEAN)
    writeError(erEXPECTATION, "<BOOLEAN EXPRESSION>");
  while (eval->value.b) {
    // зациклення UPD: походу тому що очищуєм об'єкти
    int b_val = setjmp(jump_buffer);
    if (!b_val) {
      st->whileSt->execute(st->whileSt);
    }
    if (b_val == 1) break;
    //if (b_val == 2) {} // а нам і не тре continue, і так вийде з циклу
    eval = getValueExpression(st->expression); // get new results
  }
  //free(st->expression);
  //free(st);
}
static void _print_stat(Statement *s) {
  StatementPrint *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);

  if (eval->type == dtINTEGER) // print number
    printf("%d", eval->value.i);
  if (eval->type == dtSTRING) // print string
    printf(eval->value.s);
  if (eval->type == dtBOOLEAN) // print boolean
    printf((eval->value.b)?"true":"false");
  if (eval->type == dtNIL || eval->type == dtDELETED) // print nil
    printf("nil");
  //free(st->expression); // звільняєм память щоб нас не сварили
  //free(st);
}
static void _error_stat(Statement *s) {
  StatementError *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
  if (eval->type == dtSTRING)
    printf(eval->value.s);
  else
    writeError(erEXPECTATION, "<String>");
  //free(st->expression); // звільняєм память щоб нас не сварили
  //free(st);
  exit(0);
}
static void _delete_stat(Statement *s) {
  StatementAssign *st = s->statement;
  memoryDelete(st->name);
}
static void _block_stat(Statement *s) {
  // УВАГА! не виконані statement не очищаються
  StatementBlock *st = s->statement;

  statement_node_t *current = st->list;
  Statement *tmp;
  while (current != NULL) {
    tmp = current->value;
    tmp->execute(tmp);
    current = current->next;
  }
  //free(st->list);
  //free(st);
}
static void _break_stat(Statement *s) {
  longjmp(jump_buffer, 1);
}
static void _continue_stat(Statement *s) {
  longjmp(jump_buffer, 2);
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
Statement *WhileStatement(Expression *expr, Statement *wh_st) {
  StatementWhile *st = (StatementWhile*)malloc(sizeof(StatementWhile));
  st->expression = expr;
  st->whileSt = wh_st;
  return buildStatement(stWHILE, st, &_while_stat);
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
Statement *BlockStatement() {
  StatementBlock *st = (StatementBlock*)malloc(sizeof(StatementBlock));
  st->list = statements_init();
  return buildStatement(stBLOCK, st, &_block_stat);
}
Statement *BreakStatement() {
  return buildStatement(stBREAK, NULL, &_break_stat);
}
Statement *ContinueStatement() {
  return buildStatement(stCONTINUE, NULL, &_continue_stat);
}
// statement builder
static Statement *buildStatement(StatementType st_type, void *st, void *executer) {
  Statement *s = (Statement*)malloc(sizeof(Statement));
  s->statement = st;
  s->type = st_type;
  s->execute = executer;
  return s;
}
// statements list for block statement
statement_node_t *statements_init() {
  statement_node_t *node = (statement_node_t*)malloc(sizeof(statement_node_t));
	node->value = NULL;
	node->next = NULL;
	return node;
}
void statements_push(statement_node_t *node, Statement *stat) {
  statement_node_t *current = node;

  if (current->value == NULL) { // check for first element
    current->value = stat;
    return;
  }

  while (current->next != NULL) {
      current = current->next;
  }

  current->next = (statement_node_t*)malloc(sizeof(statement_node_t));
  current->next->value = stat;
  current->next->next = NULL;
}
