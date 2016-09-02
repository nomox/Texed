#include "error.h"

write_errors = 0;
void writeError(ErrorType et, char *msg) {
	if (write_errors)
		switch (et) {
		case erNULL: printf("Error: ~%s\n", msg);
			break;
		case erVARNOTEXIST: printf("Error: ~Variable \"%s\" does not exist\n", msg);
			break;
		case erMEMORYREALLOCATION: printf("~Error: memory reallocation\n");
			break;
		case erSYNTAX: printf("Error: Lexer: Syntax\n");
			break;
		default: printf("Error: ~%s\n", msg);
			break;
		}
}