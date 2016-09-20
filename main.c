#include "stdio.h"
#include "stdlib.h"
#include "error.h"
#include "function.h"
#include "std/functions.h"

int main(int argc, char *argv[]) {
	// texed script.xrs -m=64

	write_errors = 1;

	system("chcp 1251 > nul"); // set encoding
	printf("X!!\n");

	memoryInit(1024*1024); // alloc mem.
	// register system functions
	// standart
	createSystemFunction("__print", &_print);

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

	//getch();
	return 0;
}
