#ifndef MEMORY_H
#define MEMORY_H

#include "stdbool.h"

typedef enum DATA_TYPE {
  dtDELETED,
  dtNIL,
  dtINTEGER,
  dtFLOAT,
  dtSTRING,
  dtBOOLEAN,
  dtLINK,
  dtFUNCTION
} DataType;
typedef union DATA {
  double f; // number float
  int i; // number float
  char *s; // string
  bool b; // boolean
  struct FUNCTION *fn;
  struct RECORD *link;
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

Record *memory; // dynamic memory

void memoryInit(int);
void memorySet(Record*);
Record *memoryGet(const char*);
void memoryDelete(const char*);
int memoryPosition(const char*);

Record *newRecordInteger(char*, int);
Record *newRecordFloat(char*, float);
Record *newRecordString(char*, char*);
Record *newRecordBoolean(char*, bool);
Record *newRecordNil(char*);
Record *newRecordFunction(char*, struct FUNCTION*);
Record *newRecordValue(char*, expression_value_t*);

#endif MEMORY_H
