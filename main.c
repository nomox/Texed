#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "variables.h"
#include "error.h"
#include "types.h"

int main() {
	write_errors = 1;
	/*
	char* path[80];
	getcwd(path, 80);
	printf("%s\n", path);
	*/
	printf("X!!\n");

	char fname[] = "script.xrs";
	FILE *f;
	if ((f = fopen(fname, "r")) == NULL)
	{
		printf("File not found!");
		return 1;
	}
	tokenize(f);
	fclose(f);
	//printList();
	parse();

	TokenType t;
	char kek[64];

	/*
	int addr = addVar("kek", "10", NUMBER);
	addr = addVar("v1", "1488", NUMBER);
	printf("%s\n", getVarValue("v1"));
	getVarValueByAddr(1488);
	*/

	//getch();
	return 0;
}