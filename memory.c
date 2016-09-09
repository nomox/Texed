#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "memory.h"
#include "error.h"

static int position = 0;
static int mem_size;

static int memoryEmptyPosition();

void memoryInit(int size) { // ініціалізація пам'яті !!!
  mem_size = size;
  memory = (Record*)malloc(sizeof(Record) * size);
}
void memorySet(Record *record) {
  /*
  if (record->type == dtNUMBER)
    printf("%d\n", record->data.i);
  if (record->type == dtSTRING) {
    printf(record->data.s);
    printf("\n");
  }*/
  /*
  Record *t_mem;
  t_mem = realloc(memory, position * sizeof(Record));
  if (t_mem != NULL) { // якщо вдало сформувалася память
    memory = t_mem;
    memory[position++] = *record;
    //free(t_mem);
    free(record);
  }
  else {
    writeError(erMEMORYREALLOCATION, ""); // а нє то ашібочка
  }
  */
  if (position >= mem_size)
    writeError(erMEMORYREALLOCATION, "");
  int pos = memoryPosition(record->name);
  if (pos < 0) // якщо нема то нову пишем
    memory[memoryEmptyPosition()] = *record; // memoryEmptyPosition() || position++
  else
    memory[pos] = *record;
  free(record); // destroy
}
Record *memoryGet(const char *name) {
  for (int i = 0; i < position; i++) {
    if (!strcmp(memory[i].name, name)) {
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
    if (!strcmp(memory[i].name, name)) {
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
Record *newRecordNumber(char *name, int number) {
  Record *record;
  record = (Record*)malloc(sizeof(Record));
  record->name = name;
  record->type = dtNUMBER;
  record->data.i = number;
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
