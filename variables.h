#pragma once

#define NULL 0

typedef enum VARTYPE {
	NUMBER,
	STRING,
	STRUCT,
	STYPE,
	FUNCTION,
	LINK
} VarType;

typedef struct VARIABLE {
	char *varname;
	char *varvalue;
	VarType vartype;
} Variable;

int addVar(char *name, char *value, VarType type);
void setVarValue(char *name, char *value);
void setVarValueByAddr(int addr, char *value);
int getVarAddr(char *name);
char *getVarValue(char *name);
char *getVarValueByAddr(int addr);
void removeVar(char *name);
void removeVarByAddt(int addr);
int getLastAddr();