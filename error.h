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
	erUNKNOWNLEXEM
} ErrorType;

int write_errors;
void writeError(ErrorType et, char *msg);
