#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "memory.h"
#include "types.h"
#include "error.h"

int main(int argc, char *argv[]) {
	// texed script.xrs -m=64

	write_errors = 1;

	system("chcp 1251 > nul"); // set encoding
	printf("X!!\n");

	memoryInit(1024*1024); // alloc mem. size 1 мегабайт хватить

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
	//getch();
	return 0;
}
