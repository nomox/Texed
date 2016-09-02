#include "variables.h"
#include "error.h"

#define NULL 0

int last_addr = 0;
Variable *var_bank = NULL;
Variable *varbtmp;

/*
Variable *newVariable(int size) {
	return  (Variable*)malloc(size*sizeof(Variable));
}*/
Variable *newVariable(Variable *memory,int size) {
	return  (Variable*)realloc(memory, size*sizeof(Variable));
}
int addVar(char *name, char *value, VarType type) {
	varbtmp = newVariable(var_bank, last_addr+1);

	if (varbtmp != NULL) {
		var_bank = varbtmp;
		var_bank[last_addr].varname = name;
		var_bank[last_addr].varvalue = value;
		var_bank[last_addr].vartype = type;
	}
	else {
		free(var_bank);
		writeError(erMEMORYREALLOCATION, "");
		return -1;
	}

	return last_addr++;
}
void setVarValue(char *name, char *value) {
	for (int i = 0; i < last_addr; i++)
		if (var_bank[i].varname == name) var_bank[i].varvalue = value;
		else if (i == last_addr - 1) writeError(erVARNOTEXIST, name);
}
void setVarValueByAddr(int addr, char *value) {
	var_bank[addr].varvalue = value;
}
int getVarAddr(char *name) {
	for (int i = 0; i < last_addr; i++)
		if (var_bank[i].varname == name) return i;
		else if (i == last_addr - 1) {
			writeError(erVARNOTEXIST, name);
			return -1;
		}
}
char *getVarValue(char *name) {
	for (int i = 0; i < last_addr; i++) {
		//printf("* %i %s %s\n", last_addr, var_bank[i].varname, var_bank[i].varvalue);
		if (strcmp(var_bank[i].varname, name) == 0) {
			return var_bank[i].varvalue;
		}
		else if (i == last_addr - 1) {
			writeError(erVARNOTEXIST, name);
			return "$null";
		}
	}
}
char *getVarValueByAddr(int addr) {
	if (addr<last_addr) return var_bank[addr].varvalue;
	else {
		char buffer[64];
		char var_addr[64] = "_addr:";
		itoa(addr,buffer,10);
		strcat(var_addr, buffer);
		writeError(erVARNOTEXIST, var_addr);
		return "$null";
	}
}
void removeVar(char *name) {

}
void removeVarByAddt(int addr) {

}
int getLastAddr() {
	return last_addr;
}
/*
bool checkVar(char *name) {
for (int i = 0; i < last_addr; i++) {
if (var_bank[i].varname == name) {
return true;
}
else if (i == last_addr - 1) {
return false;
}
}
}*/