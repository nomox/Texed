#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "memory.h"
#include "error.h"

static int position = 0;
static int mem_size;
/*
static Record *current_local = NULL; // parent record for storing
static Record *local_record;*/

static memory_node_t *getNodeByName(memory_node_t*, char*);
static void mem_list_push(memory_node_t*, Record*);

void memoryInit(int size) { // ініціалізація пам'яті !!!
  // init list
  memory_handler = mem_list_init();
  mem_list_push(memory_handler, newRecordNil("$"));
  current_handler = memory_handler;
}
void memorySet(Record *record) {
  memory_node_t *node = getNodeByName(current_handler, record->name);
  if (node != NULL) {
    node->record = record; // mb need to clear record and assign it again
  }
  else {
    mem_list_push(current_handler, record);
  }
  //free(record);
}
Record *memoryGet(const char *name) {
  memory_node_t *node = getNodeByName(current_handler, name);
  if (node != NULL) {
    return node->record;
  }
  else
    writeError(erVARNOTEXIST, name);
}
void memoryDelete(const char *name) {
  memory_node_t *current = current_handler;
  bool exist = false;
  Record *r;
  Record *r_del;
  while (current != NULL) {
    if (current->next != NULL)
    r = current->next->record;
    if (current != NULL && !strcmp(r->name, name)) { // !можливо потрібно перевірити екземпляр Record
      exist = true;
      r_del = current->next;
      if (current->next->next != NULL)
        current->next = current->next->next;
      else
        current->next = NULL;
      break;
    }
    current = current->next; // next node
  }
  if (!exist)
    writeError(erVARNOTEXIST, name);
  free(r_del);
}

static memory_node_t *getNodeByName(memory_node_t *node, char *name) {
  memory_node_t *current = node;
  Record *r;
  while (current != NULL) {
    r = current->record;
    if (r != NULL && !strcmp(r->name, name)) { // !можливо потрібно перевірити екземпляр Record
      return current;
    }
    current = current->next; // next node
  }
  return NULL; // якщо нічого не найдено
}

// for linked list
memory_node_t *mem_list_init() {
  memory_node_t *node = (memory_node_t*)malloc(sizeof(memory_node_t));
	node->record = NULL;
	node->next = NULL;
	return node;
}
static void mem_list_push(memory_node_t *node, Record *rec) {
  memory_node_t *current = node;
  if (current->record == NULL) { // check for first element
    current->record = rec;
    return;
  }
  while (current->next != NULL) {
      current = current->next;
  }
  current->next = (memory_node_t*)malloc(sizeof(memory_node_t));
  current->next->record = rec;
  current->next->next = NULL;
}
// mem stack
handler_node_t *handler_list_init() {
  handler_node_t *node = (handler_node_t*)malloc(sizeof(handler_node_t));
	node->handler = NULL;
	node->next = NULL;
	return node;
}
void handler_push(handler_node_t *node, memory_node_t *mem) {
  handler_node_t *current = node;
  if (current->handler == NULL) { // check for first element
    current->handler = mem;
    return;
  }
  while (current->next != NULL) {
      current = current->next;
  }
  current->next = (handler_node_t*)malloc(sizeof(handler_node_t));
  current->next->handler = mem;
  current->next->next = NULL;
}
memory_node_t *handler_get_last(handler_node_t *node) {
  handler_node_t *current = node;
  while (current->next != NULL)
    current = current->next;
  return current->handler;
}
memory_node_t *handler_pop(handler_node_t *node) {
  handler_node_t *current = node;
  printf("HELLO\n");
  if (current->next == NULL)
    return current->handler;
  printf("PSHEL VON\n");
  while (current->next->next != NULL)
    current = current->next;
  free(current->next);
  current->next = NULL;
  return current->handler;
}
// tools
Record *newRecordInteger(char *name, int number) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtINTEGER;
  record->data.i = number;
  return record;
}
Record *newRecordFloat(char *name, float number) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtFLOAT;
  record->data.f = number;
  return record;
}
Record *newRecordString(char *name, char* string) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtSTRING;
  record->data.s = string;
  return record;
}
Record *newRecordBoolean(char *name, bool boolean) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtBOOLEAN;
  record->data.b = boolean;
  return record;
}
Record *newRecordNil(char *name) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtNIL;
  return record;
}
Record *newRecordFunction(char *name, struct FUNCTION *func) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtFUNCTION;
  record->data.fn = func;
  return record;
}
Record *newRecordTable(char *name, memory_node_t *table) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtTABLE;
  record->data.table = table;
  return record;
}
Record *newRecordValue(char *name, expression_value_t *val) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = (DataType)val->type;
  record->data = (Data)val->value;
  return record;
}
