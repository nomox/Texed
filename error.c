#include "stdio.h"
#include "stdlib.h"
#include "error.h"
#include "tokens.h"

void writeError(ErrorType et, const char *msg) {
	if (write_errors)
		switch (et) {
		case erVARNOTEXIST: printf("\nError: \"%s\" not defined (at line %d)", msg, getLine());
			break;
		case erMEMORYREALLOCATION: printf("\n~Error: memory reallocation");
			break;
		case erSYNTAX: printf("\nError: Syntax");
			break;
		case erZERODIVISION: printf("\nError: Zero division (at line %d)", getLine());
				break;
		case erEXPECTATION: printf("\nError: '%s' expected (at line %d)", msg, getLine());
				break;
		case erUNKNOWNLEXEM: printf("\nError: Unknown lexem '%s' (at line %d)", msg, getLine());
				break;
		case erUNKNOWNFUNC: printf("\nError: '%s' not an function (at line %d)", msg, getLine());
				break;
		case erNOTANARRAY: printf("\nError: '%s' not an array (at line %d)", msg, getLine());
				break;
		case erOUTOFRANGE: printf("\nError: '%s' array out of range (at line %d)", msg, getLine());
				break;
		case erFUNCNOTINIT: printf("\nError: function '%s' not initialized (at line %d)", msg, getLine());
				break;
		case erARGMISMATCH: printf("\nError: arguments mismatch for function '%s' (at line %d)", msg, getLine());
				break;
		case erOUTOFMEMORY: printf("\nError: Out of memory");
				break;
		case erNULL:
		default: printf("\nError: %s", msg);
			break;
		}
	exit(0);
}
