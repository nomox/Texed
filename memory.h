#ifndef MEMORY_H
#define MEMORY_H

#include "stdbool.h"

typedef enum DATA_TYPE {
  dtTABLE,
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
typedef struct handler_node {
  memory_node_t *handler;
  struct handler_node *next;
} handler_node_t;

handler_node_t *handler_list_init();
void handler_push(handler_node_t*, memory_node_t*);
memory_node_t *handler_get_last(handler_node_t*);
memory_node_t *handler_pop(handler_node_t*);

memory_node_t *memory_handler; // root handler
memory_node_t *current_handler;
memory_node_t *restore_handler;
//handler_node_t *mem_stack;

void memoryInit(int);
void memorySet(Record*);
Record *memoryGet(const char*);
void memoryDelete(const char*);
int memoryPosition(const char*);

memory_node_t *mem_list_init();

Record *newRecordInteger(char*, int);
Record *newRecordFloat(char*, float);
Record *newRecordString(char*, char*);
Record *newRecordBoolean(char*, bool);
Record *newRecordNil(char*);
Record *newRecordFunction(char*, struct FUNCTION*);
Record *newRecordTable(char*, memory_node_t*);
Record *newRecordValue(char*, expression_value_t*);

#endif MEMORY_H
