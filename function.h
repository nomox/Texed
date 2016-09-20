#ifndef FUNCTION_H
#define FUNCTION_H

#include "expression.h"
#include "statement.h"

typedef struct FUNCTION {
  char *arg_template; // exapmple: "a;b;c:integer", system function "$"
  union {
    Statement *stat;
    void* (*func)(struct expression_value**, int);
  };
} Function;

void createSystemFunction(char*, void*);
void createFunction(char*, char*, Statement*);
struct expression_value *callFunction(char*, struct expression_value**, int);

#endif FUNCTION_H
