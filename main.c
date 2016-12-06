#include "stdio.h"
#include "stdlib.h"
#include "error.h"
#include "function.h"
#include "std/functions.h"

int main(int argc, char *argv[]) {
	// texed script.xrs -m=64

	write_errors = 1;

	system("chcp 1251 > nul"); // set encoding
	printf("X++ Lang 0.4.1\n\n");

	memoryInit(); // alloc mem.
	// register system functions
	// standart
	createSystemFunction("__print", &_print); // __print(value..)
	createSystemFunction("__read", &_read); // __read() return string
	createSystemFunction("__listsize", &_listsize); // __listsize(list) return int size
	createSystemFunction("__listcreate", &_listcreate); // __listcreate(size) return list
	createSystemFunction("__listpush", &_listpush); // __listset(list, value)
	createSystemFunction("__listremove", &_listremove); // __listremove(list, index)
	createSystemFunction("__listisempty", &_listisempty); // __listisempty(list) return boolean empty
	createSystemFunction("__strlength", &_strlength); // __strlength(str) return int size
	createSystemFunction("__strcat", &_strcat); // __strcat(str1, str2) return string
	createSystemFunction("__strtoint", &_strtoint); // __strtoint(str) return int
	createSystemFunction("__strtonumber", &_strtonumber); // __strtoint(str) return int
	createSystemFunction("__sin", &_sin); // __sin(number) return number
	createSystemFunction("__cos", &_cos); // __cos(number) return number
	createSystemFunction("__sqrt", &_sqrt); // __sqrt(number) return number
	createSystemFunction("__time", &_time); // __time() return number
	createSystemFunction("__system", &_system); // __system(string)

	// open file
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
