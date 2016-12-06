#pragma once

typedef enum ERRORTYPE {
	erNULL,
	erVARNOTEXIST,
	erERRORVARISNULL,
	erMEMORYREALLOCATION,
	erOUTOFMEMORY,
	erSYNTAX,
	erZERODIVISION,
	erEXPECTATION,
	erUNKNOWNLEXEM,
	erUNKNOWNFUNC,
	erNOTANARRAY,
	erFUNCNOTINIT,
	erARGMISMATCH,
	erOUTOFRANGE
} ErrorType;

int write_errors;
void writeError(ErrorType et, const char *msg);
