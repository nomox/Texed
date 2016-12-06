#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "error.h"
#include "function.h"
#include "memory.h"

void createSystemFunction(char *name, void *s_func) {
  Function *fn = (Function*)malloc(sizeof(Function));
  fn->arg_template = malloc(1);
  strcpy(fn->arg_template, "$"); // сист. функція
  fn->func = s_func;
  memorySet(memory_handler, newRecordFunction(name, fn));
}
void createFunction(PreparedVariable var, char* arg_template, Statement *stat) {
  Function *fn = (Function*)malloc(sizeof(Function));
  fn->arg_template = arg_template;
  fn->stat = stat;
  memorySet(var.parent_handler, newRecordFunction(var.name, fn));
}
struct expression_value *callFunction(PreparedVariable var, struct expression_value *args[], int arg_len) { // return value
  Record *r = memoryGet(var.parent_handler, var.name); // берем функцію з памяті
  if (r->type != dtFUNCTION) // перевіряєм чи то функція
    writeError(erUNKNOWNFUNC, r->name);
  Function *fn = r->data.fn;
  if (!strcmp(fn->arg_template, "$")) { // якщо системна функція
    return fn->func(args, arg_len);
  }
  else {
    if (fn->stat == NULL) { // якщо функція не ініціалізована
      writeError(erFUNCNOTINIT, r->name);
    }
    if (!setjmp(jump_return)) {
      handler_stack_push(&handler_stack, scope_handler); // store
      scope_handler = var.parent_handler; // go to function parent scope
      // init args
      int c_arg = 0;
      char *tok = strtok(fn->arg_template, ";"); // name of function argument
      while (tok != NULL) {
        memorySet(scope_handler, newRecordValue(tok, *(args+c_arg))); // address of elemnt
        c_arg++;
        tok = strtok(NULL, ";");
      }
      if (arg_len != c_arg)
        writeError(erARGMISMATCH, r->name);
      // start statement

      fn->stat->execute(fn->stat); // can return
      scope_handler = handler_stack_pop(&handler_stack); // restore
      return valueNil();
    }
    else {
      // if (returned_val == NULL)
      scope_handler = handler_stack_pop(&handler_stack); // restore
      return returned_val;
    }
  }
  return valueNil();
}
