#ifndef MEMORY_H
#define MEMORY_H

#include "stdlib.h"
#include "stdbool.h"

typedef enum DATA_TYPE {
  dtTABLE,
  dtLIST,
  dtLINK,
  dtNIL,
  dtINTEGER,
  dtFLOAT,
  dtSTRING,
  dtBOOLEAN,
  dtFUNCTION
} DataType;
typedef union DATA {
  double f; // number float
  int i; // number float
  char *s; // string
  bool b; // boolean
  struct FUNCTION *fn;
  struct RECORD *link;
  struct list_node *list;
  struct memory_node *table;
} Data;

typedef struct expression_value {
  DataType type;
  Data value;
} expression_value_t;

typedef struct RECORD {
  DataType type;
  Data data;
  struct RECORD *parent;
  char *name;
} Record;
// memory header
typedef struct memory_node {
  Record *record;
  struct memory_node *next;
} memory_node_t;
// stack list node
typedef struct HANDLER_STACK {
  memory_node_t *value;
  struct HANDLER_STACK *next;
} handler_stack_t;

typedef struct list_node {
  expression_value_t *value;
  char *key;
  struct list_node *next;
} list_node_t;

typedef struct PREPARED_VARIABLE {
	memory_node_t *parent_handler;
	char *name;
} PreparedVariable;

memory_node_t *memory_handler; // root handler
memory_node_t *scope_handler;
handler_stack_t *handler_stack;
//handler_node_t *mem_stack;

void mem_list_push(memory_node_t**, Record*);

void memoryInit();
void memorySet(memory_node_t*, Record*);
Record *memoryGet(memory_node_t*, const char*);
void memoryDelete(memory_node_t*, const char*);
//int memoryPosition(const char*);

void handler_stack_push(handler_stack_t**, memory_node_t*);
memory_node_t *handler_stack_pop(handler_stack_t**);

void list_push(list_node_t**, expression_value_t*);
bool list_empty(list_node_t*);
expression_value_t *list_get(list_node_t*, unsigned int);
bool list_set(list_node_t*, unsigned int, expression_value_t*);
bool list_remove(list_node_t**, int);
int list_size(list_node_t*);
bool list_empty(list_node_t*);

PreparedVariable prepareVariable(char*);

Record *newRecordInteger(char*, int);
Record *newRecordFloat(char*, float);
Record *newRecordString(char*, char*);
Record *newRecordBoolean(char*, bool);
Record *newRecordNil(char*);
Record *newRecordFunction(char*, struct FUNCTION*);
Record *newRecordTable(char*, memory_node_t*);
Record *newRecordList(char*, list_node_t*);
Record *newRecordValue(char*, expression_value_t*);

#endif MEMORY_H
