#include "error.h"

write_errors = 0;
void writeError(ErrorType et, char *msg) {
	if (write_errors)
		switch (et) {
		case erVARNOTEXIST: printf("Error: ~Variable \"%s\" does not exist\n", msg);
			break;
		case erMEMORYREALLOCATION: printf("~Error: memory reallocation\n");
			break;
		case erSYNTAX: printf("Error: Lexer: Syntax\n");
			break;
		case erZERODIVISION: printf("Error: Zero division\n");
				break;
		case erEXPECTATION: printf("Error: '%s' expected\n", msg);
				break;
		case erUNKNOWNLEXEM: printf("Error: Unknown lexem '%s'\n", msg);
				break;
		case erNULL:
		default: printf("Error: ~%s\n", msg);
			break;
		}
	exit(0);
}
