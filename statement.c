#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "error.h"
#include "statement.h"
#include "expression.h"
#include "function.h"
#include "memory.h"

static Statement *buildStatement();

static jmp_buf jump_buffer; // 1 - break, 2 - continue
bool loop_begin = false;

static void _assign_stat(Statement *s) {
  StatementAssign *st = s->statement;
  PreparedVariable var = prepareVariable(st->name);
  expression_value_t *eval = getValueExpression(st->expression);
  // assign controller
  if (eval->type == dtINTEGER)
    memorySet(var.parent_handler, newRecordInteger(var.name, eval->value.i));
  if (eval->type == dtFLOAT)
    memorySet(var.parent_handler, newRecordFloat(var.name, eval->value.f));
  if (eval->type == dtSTRING)
    memorySet(var.parent_handler, newRecordString(var.name, eval->value.s));
  if (eval->type == dtBOOLEAN)
    memorySet(var.parent_handler, newRecordBoolean(var.name, eval->value.b));
  if (eval->type == dtNIL)
    memorySet(var.parent_handler, newRecordNil(var.name));
  if (eval->type == dtTABLE)
    memorySet(var.parent_handler, newRecordTable(var.name, eval->value.table));
  if (eval->type == dtLIST)
    memorySet(var.parent_handler, newRecordList(var.name, eval->value.list));
  if (eval->type == dtFUNCTION)
    memorySet(var.parent_handler, newRecordFunction(var.name, eval->value.fn));
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
static void _for_stat(Statement *s) {
  StatementFor *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expr);
  if (eval->type != dtLIST)
    writeError(erEXPECTATION, "<list>");
  for (int i = 0; i < list_size(eval->value.list); i++) {
    memorySet(scope_handler, newRecordValue(st->name, list_get(eval->value.list, i)));
    int b_val = setjmp(jump_buffer);
    if (!b_val) {
      st->forSt->execute(st->forSt);
    }
    if (b_val == 1) break;
  }
}
static void _print_stat(Statement *s) {
  StatementPrint *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
  if (eval->type == dtINTEGER) // print number
    printf("%d", eval->value.i);
  if (eval->type == dtFLOAT) // print number
    printf("%g", eval->value.f);
  if (eval->type == dtSTRING) // print string
    printf(eval->value.s);
  if (eval->type == dtBOOLEAN) // print boolean
    printf((eval->value.b)?"<true>":"<false>");
  if (eval->type == dtNIL) // print nil
    printf("<nil>");
  if (eval->type == dtTABLE) // print table
    printf("<table>");
  if (eval->type == dtLIST) // print list
    printf("<list>");
  if (eval->type == dtFUNCTION) // print function
    printf("<fuction>");
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
  StatementDelete *st = s->statement;
  PreparedVariable var = prepareVariable(st->name);
  memoryDelete(var.parent_handler, var.name);
}
static void _block_stat(Statement *s) {
  // УВАГА! не виконані statement не очищаються
  StatementBlock *st = s->statement;

  statement_node_t *current = st->list;
  Statement *tmp;
  while (current != NULL) {
    tmp = current->value;
    if (tmp == NULL)
      break;
    tmp->execute(tmp);
    current = current->next;
  }
  // TODO: clear list
}
static void _break_stat(Statement *s) {
  longjmp(jump_buffer, 1);
}
static void _continue_stat(Statement *s) {
  longjmp(jump_buffer, 2);
}
static void _funcdef_stat(Statement *s) {
  StatementFuncDef *st = s->statement;
  PreparedVariable var = prepareVariable(st->name);
  createFunction(var, st->args, st->func_body);
}
static void _return_stat(Statement *s) {
  StatementReturn *st = s->statement;
  if (st->expression != NULL)
    returned_val = getValueExpression(st->expression);
  else {
    returned_val = valueNil();
  }
  longjmp(jump_return, 1);
}
static void _istrue_stat(Statement *s) {
  StatementIsTrue *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
  if (eval->type != dtBOOLEAN)
    writeError(erEXPECTATION, "<BOOLEAN EXPRESSION>");
  bool res = eval->value.b;
  if (res) {
    st->statement->execute(st->statement);
  }
  //free(st->expression);
  //free(st);
}
static void _isfalse_stat(Statement *s) {
  StatementIsFalse *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
  if (eval->type != dtBOOLEAN)
    writeError(erEXPECTATION, "<BOOLEAN EXPRESSION>");
  bool res = eval->value.b;
  if (!res) {
    st->statement->execute(st->statement);
  }
  //free(st->expression);
  //free(st);
}
static void _default_stat(Statement *s) {
  StatementDefault *st = s->statement;
  expression_value_t *eval = getValueExpression(st->expression);
}
static void _table_stat(Statement *s) {
  StatementTable *st = s->statement;
  PreparedVariable var = prepareVariable(st->name);
  memory_node_t *table = NULL;
  mem_list_push(&table, newRecordNil("$"));
  Record *rec = newRecordTable(var.name, table); // create new table
  memorySet(var.parent_handler, rec); // push to current scope
  memory_node_t *tmp = scope_handler;
  scope_handler = rec->data.table;
  memorySet(scope_handler, newRecordTable("_parent", var.parent_handler)); // set parent table link TODO create link record
  handler_stack_push(&handler_stack, tmp); // restore handler
}
static void _closetable_stat(Statement *s) {
  scope_handler = handler_stack_pop(&handler_stack); // restore handler
}
static void _arrayassign_stat(Statement *s) {
  StatementArrayAssign *st = s->statement;
  PreparedVariable var = prepareVariable(st->name);
  Record *r = memoryGet(var.parent_handler, var.name);
  if (r->type != dtLIST)
    writeError(erNOTANARRAY, var.name);
  list_node_t *list = r->data.list; // get list

  expression_value_t *idxValue = NULL; // current index
  expression_value_t *elValue = NULL; // elemnt value
  expression_node_t *current; // current expression
  int i;
  for (i = 0, current = st->indices; current != NULL; i++, current = current->next) {
    if (i > 0) { // skip first iteration
      elValue = list_get(list, (unsigned int)idxValue->value.list);
      if (elValue == NULL)
        writeError(erOUTOFRANGE, var.name);
      if (elValue->type != dtLIST) // check for list
        writeError(erNOTANARRAY, var.name);
      list = elValue->value.list;
    }
    idxValue = getValueExpression(current->value); // get index
    if (idxValue->type != dtINTEGER) // index is value of integer
      writeError(erEXPECTATION, "<Index> (integer)");
  }
  // たび ぴすだ
  expression_value_t *value = getValueExpression(st->expression);
  bool result = list_set(list, (unsigned int)idxValue->value.list, value);
  if (result == false)
    writeError(erOUTOFRANGE, var.name);
}

// statement decl
Statement *AssignStatement(char *name, Expression *expr) {
  //Statement *s = (Statement*)malloc(sizeof(Statement));
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
Statement *ForStatement(char *name, Expression *expr, Statement *forSt) {
  StatementFor *st = (StatementFor*)malloc(sizeof(StatementFor));
  st->name = name;
  st->expr = expr;
  st->forSt = forSt;
  return buildStatement(stFOR, st, &_for_stat);
}
Statement *PrintStatement(Expression *expr) {
  StatementPrint *st = (StatementPrint*)malloc(sizeof(StatementPrint));
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
Statement *FuncDefStatement(char *name, char *arg_template, Statement *func_body) {
  StatementFuncDef *st = (StatementFuncDef*)malloc(sizeof(StatementFuncDef));
  st->name = malloc(strlen(name));
  strcpy(st->name, name);
  st->args = malloc(strlen(arg_template));
  strcpy(st->args, arg_template);
  st->func_body = func_body;
  return buildStatement(stFUNCDEF, st, &_funcdef_stat);
}
Statement *ReturnStatement(Expression *expr) {
  StatementReturn *st = (StatementReturn*)malloc(sizeof(StatementReturn));
  st->expression = expr;
  return buildStatement(stRETURN, st, &_return_stat);
}
Statement *IfTrueStatement(Expression *expr, Statement *stat) {
  StatementIsTrue *st = (StatementIsTrue*)malloc(sizeof(StatementIsTrue));
  st->expression = expr;
  st->statement = stat;
  return buildStatement(stISTRUE, st, &_istrue_stat);
}
Statement *IfFalseStatement(Expression *expr, Statement *stat) {
  StatementIsFalse *st = (StatementIsFalse*)malloc(sizeof(StatementIsFalse));
  st->expression = expr;
  st->statement = stat;
  return buildStatement(stISFALSE, st, &_isfalse_stat);
}
Statement *DefaultStatement(Expression *expr) {
  StatementDefault *st = (StatementDefault*)malloc(sizeof(StatementDefault));
  st->expression = expr;
  return buildStatement(stDEFAULT, st, &_default_stat);
}
Statement *TableStatement(char *name) {
  StatementTable *st = (StatementTable*)malloc(sizeof(StatementTable));
  st->name = malloc(strlen(name));
  strcpy(st->name, name);
  //free(name); // clear allocated in variableGet()
  return buildStatement(stTABLE, st, &_table_stat);
}
Statement *CloseTableStatement() {
  return buildStatement(stCLOSETABLE, NULL, &_closetable_stat);
}
Statement *ArrayAssignStatement(char *name, expression_node_t *indices, Expression *expr) {
  //Statement *s = (Statement*)malloc(sizeof(Statement));
  StatementArrayAssign *st = (StatementArrayAssign*)malloc(sizeof(StatementArrayAssign));
  st->name = name;
  st->indices = indices;
  st->expression = expr;
  return buildStatement(stASSIGN, st, &_arrayassign_stat);
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
