#pragma once

#include "stdbool.h"

typedef enum DATA_TYPE {
  dtDELETED,
  dtNIL,
  dtNUMBER,
  dtSTRING,
  dtBOOLEAN,
  dtLINK
} DataType;
typedef union DATA {
  float f; // number float
  int i; // number float
  char *s; // string
  bool b; // boolean
  struct RECORD *parrent;
} Data;
typedef struct RECORD {
  DataType type;
  Data data;
  char *name;
} Record;

Record *memory; // dynamic memory

void memoryInit(int);
void memorySet(Record*);
Record *memoryGet(const char*);
void memoryDelete(const char*);
int memoryPosition(const char*);
Record *newRecordNumber(char*, int);
Record *newRecordString(char*, char*);
