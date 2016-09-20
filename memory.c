#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "memory.h"
#include "error.h"

static int position = 0;
static int mem_size;
static Record *current_local = NULL; // parent record for storing
static Record *local_record;

static int memoryEmptyPosition();

void memoryInit(int size) { // ініціалізація пам'яті !!!
  mem_size = size;
  memory = (Record*)malloc(sizeof(Record) * size);

  Record *local_record = (Record*)malloc(sizeof(Record));
  local_record->name = malloc(1);
  local_record->name = "$";
  local_record->type = dtLINK;
  memorySet(local_record);
}
/*
get root

*/
void memorySet(Record *record) {
  if (position >= mem_size)
    writeError(erMEMORYREALLOCATION, "");
  int pos = memoryPosition(record->name); // перевіряєм чи є така змінна
  if (pos < 0) { // якщо нема то нову пишем
    int new_pos = memoryEmptyPosition();
    memory[new_pos] = *record; // створюэмо нову змінну
    memory[new_pos].parent = current_local;
  }
  else {
    Record *tmp = memory[pos].parent;
    memory[pos] = *record;
    memory[pos].parent = tmp;
  }
  free(record); // destroy
}
Record *memoryGet(const char *name) {
  for (int i = 0; i < position; i++) {
    if (!strcmp(memory[i].name, name) && current_local == memory[i].parent) {
      Record *r = (Record*)malloc(sizeof(Record));
      *r = memory[i];
      return r;
    }
  }
  for (int i = 0; i < position; i++) {
    if (!strcmp(memory[i].name, name) && memory[i].parent == NULL) {
      Record *r = (Record*)malloc(sizeof(Record));
      *r = memory[i];
      return r;
    }
  }
  writeError(erVARNOTEXIST, name);
}
void memoryDelete(const char *name) {
  for (int i = 0; i < position; i++) {
    if (!strcmp(memory[i].name, name)) {
      memory[i].type = dtDELETED;
      return;
    }
  }
  writeError(erVARNOTEXIST, name);
}
int memoryPosition(const char *name) {
  for (int i = 0; i < position; i++) {
    if (!strcmp(memory[i].name, name) && current_local == memory[i].parent) {
      return i;
    }
  }
  return -1;
}
static int memoryEmptyPosition() {
  for (int i = 0; i < position; i++) {
    if (memory[i].type == dtDELETED) {
      return i;
    }
  }
  return position++;
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
Record *newRecordValue(char *name, expression_value_t *val) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = (DataType)val->type;
  record->data = (Data)val->value;
  return record;
}
