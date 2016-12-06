#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "memory.h"
#include "error.h"

#define VAR_SEPARATOR "."

static memory_node_t *getNodeByName(memory_node_t*, const char*);

void memoryInit() { // ініціалізація пам'яті !!!
  // init list
  memory_handler = NULL;
  mem_list_push(&memory_handler, newRecordNil("$"));
  scope_handler = memory_handler;

  handler_stack = NULL;
}
void memorySet(memory_node_t *parent_handler, Record *record) {
  /**
	* @param parent_handler - parent table, record - new record
	*/
  memory_node_t *node = getNodeByName(parent_handler, record->name);
  if (node != NULL) {
    node->record = record; // check for assign type
  }
  else {
    mem_list_push(&parent_handler, record);
  }
  //free(record);
}
Record *memoryGet(memory_node_t *parent_handler, const char *name) {
  /**
	* @param parent_handler - parent table, name - variable name
  * @return Record
	*/
  memory_node_t *node = getNodeByName(parent_handler, name);
  if (node != NULL) {
    return node->record;
  }
  else
    writeError(erVARNOTEXIST, name);
  return NULL;
}
void memoryDelete(memory_node_t *parent_handler, const char *name) {
  /**
	* @param parent_handler - parent table, name - variable name
	*/
  memory_node_t *current = parent_handler;
  bool exist = false;
  Record *r;
  Record *r_del;
  while (current != NULL) {
    if (current->next != NULL)
    r = current->next->record;
    if (current != NULL && !strcmp(r->name, name)) { // !можливо потрібно перевірити екземпляр Record
      // TODO compare pointer address
      exist = true;
      r_del = current->record; // FIXED!
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

static memory_node_t *getNodeByName(memory_node_t *node, const char *name) {
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
void mem_list_push(memory_node_t **node, Record *rec) {
  if (*node == NULL) {
    *node = malloc(sizeof(memory_node_t));
    (*node)->record = rec;
    (*node)->next = NULL;
    return;
  }
  memory_node_t *current = *node;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = malloc(sizeof(memory_node_t));
  current->next->record = rec;
  current->next->next = NULL;
}
// mem stack
void handler_stack_push(handler_stack_t **head, memory_node_t *value) {
  handler_stack_t *temp = (handler_stack_t*)malloc(sizeof(handler_stack_t));
  temp->value = value;
  temp->next = *head;
  *head = temp;
}
memory_node_t *handler_stack_pop(handler_stack_t **head) {
  if (*head == NULL) {
    printf("Stack is empty\n");
    exit(0);
  }
  memory_node_t *item;
  handler_stack_t *temp = *head;
  item = (*head)->value;
  *head = (*head)->next;
  free(temp);
  return item;
}

// list

void list_push(list_node_t **head, expression_value_t *value) {
  /**
	* @param head - list handler, value
	*/
  if (list_empty(*head)) {
    *head = malloc(sizeof(list_node_t));
    (*head)->value = value;
    (*head)->next = NULL;
    return;
  }
  list_node_t *current = *head;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = malloc(sizeof(list_node_t));
  current->next->value = value;
  current->next->next = NULL;
}
expression_value_t *list_get(list_node_t *head, unsigned int pos) {
  /**
	* @param head - list handler, pos - postion of element
  * @return value, NULL if out of range
	*/
  int length = 0;
  for(list_node_t *current = head; current != NULL; current = current->next, length++) {
    if (length == pos)
      return current->value;
  }
  return NULL;
}
bool list_set(list_node_t *head, unsigned int pos, expression_value_t *value) {
  /**
	* @param head - list handler, pos - postion of element
  * @return true if success, false if out of range
	*/
  int length = 0;
  for(list_node_t *current = head; current != NULL; current = current->next, length++) {
    if (length == pos) {
      current->value = value;
      return true;
    }
  }
  return false;
}
bool list_remove(list_node_t **head, int pos) {
  /**
	* @param head - list handler, pos - postion of element
  * @return true if success
	*/
  list_node_t *current = *head;
  list_node_t *temp_node = NULL;

  for (int i = 0; i < pos-1; i++) {
    if (current->next == NULL) {
      return false;
    }
    current = current->next;
  }
  temp_node = current->next;
  current->next = temp_node->next;
  free(temp_node);
  return true;
}
int list_size(list_node_t *head) {
  /**
	* @param head - list handler
  * @return size of list, elements count
	*/
  int length = 0;
  for(list_node_t *current = head; current != NULL; current = current->next, length++);
  return length;
}
bool list_empty(list_node_t *head) {
  /**
	* @param head - list handler
  * @return true if list is empty
	*/
  return (head == NULL);
}

// get PreparedVariable
PreparedVariable prepareVariable(char *name) {
  PreparedVariable var;
  memory_node_t *current_table = scope_handler;
  char *tok = strtok(name, VAR_SEPARATOR);
  char *last = tok;
  Record *r = NULL;
  while ((tok = strtok(NULL, VAR_SEPARATOR)) != NULL) {
		r = memoryGet(current_table, last); // get table
		if (r->type != dtTABLE) // for first iteration
			writeError(erEXPECTATION, "<TABLE>");
		current_table = r->data.table;
    last = tok;
  }
  var.parent_handler = current_table;
	var.name = last;
  return var;
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
Record *newRecordList(char *name, list_node_t *list) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtLIST;
  record->data.list = list;
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
