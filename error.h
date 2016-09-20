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
	erFUNCNOTINIT,
	erARGMISMATCH
} ErrorType;

int write_errors;
void writeError(ErrorType et, char *msg);
