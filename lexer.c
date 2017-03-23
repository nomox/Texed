#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "ctype.h"
#include "types.h"
#include "tokens.h"
#include "error.h"

#define LN_SIZE 512 // max line size
#define VAL_SIZE 64 // max value size
#define VAL_EMPTY ""

void tokenize(FILE *f);
static void tokenizeLine(char*);
static void tokenizeWord();
static void tokenizeString();
static void tokenizeNumber();
static void tokenizeHex();
static bool isAvailableName(char);
static bool isAvailableNumber(char);
static bool isAvailableHexNumber(char);
static int indexOf(const char*, const char);
static TokenType getTokenByChar(const char c);
static TokenType getTokenByWord(const char *s);

static enum {numINT, numFLOAT}; // number types

static const char OPERATION_CHARS[] = "+-*/:.,()[]{}#=&$%@!?<>";
static const TokenType OPERATION_TOKEN[] = {
	ttPLUS,
	ttMINUS,
	ttMULTIPLY,
	ttDIVIDE,
	ttEXTEND,
	ttDOT,
	ttCOMMA,
	ttPOPEN,
	ttPCLOSE,
	ttSQOPEN,
	ttSQCLOSE,
	ttOPENBLOCK,
	ttCLOSEBLOCK,
	ttGHOST,
	ttASSIGN,
	ttPUBLIC,
	ttPRIVATE,
	ttPACK,
	ttEVENT,
	ttNOT,
	ttISTRUE,
	ttLT,
	ttGT
};
static const char *WORD_STRINGS[] = {
	"if",
	"else",
	"switch",
	"case",
	"or",
	"and",
	"for",
	"while",
	"do",
	"function",
	"fn",
	"this",
	"self",
	"use",
	"nil",
	"true",
	"false",
	"print",
	"error",
	"delete",
	"break",
	"continue",
	"return",
	"in"
};
static const TokenType WORD_TOKEN[] = {
	ttIF,
	ttELSE,
	ttSWITCH,
	ttCASE,
	ttOR,
	ttAND,
	ttFOR,
	ttWHILE,
	ttDO,
	ttFUNCTION,
	ttFNDEF,
	ttTHIS,
	ttSELF,
	ttUSE,
	ttNIL,
	ttTRUE,
	ttFALSE,
	ttPRINT,
	ttERROR,
	ttDELETE,
	ttBREAK,
	ttCONTINUE,
	ttRETURN,
	ttIN
};
static bool skip = false;
static int nline = 0;
static char *source;
static int position = 0;
static TokenType tt;

void tokenize(FILE *f) {
	char tmp_line[LN_SIZE];
	while (fgets(tmp_line, LN_SIZE, f) != NULL) {
		tokenizeLine(tmp_line);
		//printf("line: %d\n", nline);
	}
	addToken(ttEOF, "");
}

static void tokenizeLine(char *line) {
	source = line;
	nline++;
	while (source[position] != '\0') { // поки не символ закінчення
		// обробка коментарів
		if (source[position] == '/' && source[position + 1] == '/') {
			break; //  якщо коментар // то пропускаєм строку
		}
		else if (source[position] == '*' && source[position + 1] == '/') { // закрити багаторядковий коментар
			skip = false;
			position += 2;
		}
		else if (source[position] == '/' && source[position + 1] == '*') {
			skip = true;
			position += 2;
		}
		if (skip) { // if comment - skeep it
			position++;
			continue;
		}
		/*-----------------------------------------------------------------*/
		if (isalpha(source[position]) || source[position] == '_') { // checking for variables
			tokenizeWord();
		}
		else if (source[position] == '0' && source[position + 1] == 'x') {
			tokenizeHex();
		}
		else if (isdigit(source[position])) { // value
			tokenizeNumber();
		}
		else if (source[position] == '\"') { // string
			tokenizeString();
		}
		else if (tt = getTokenByChar(source[position])) { // operation
			if (tt != ttNULL)
				addToken(tt, "");
			position++;
		}
		else {
			position++; // якщо попадаєть лєва шняга то пропускаєм
		}
	}
	//if (position > 0 && !skip)
	addToken(ttEOL, VAL_EMPTY); // end of line
	position = 0;
}
static void tokenizeWord() {
	TokenType tt;
	char buffer[VAL_SIZE] = "";
	int bufpos = 0;
	while (isAvailableName(source[position])) {
		buffer[bufpos++] = source[position++];
	}

	tt = getTokenByWord(buffer);
	if (tt != ttNULL) {
		addToken(tt, "");
	}
	else
		addToken(ttVARIABLE, buffer);
}
static void tokenizeString() {
	char buffer[VAL_SIZE] = "";
	int bufpos = 0;
	position++;
	for (;;) {
		// чекаєм чи є спец символи тіпа \n \t \\ \"
		if (source[position] == '\\') {
			position++; // пропускаєм ту фігню
			switch (source[position]) {
			case '"': buffer[bufpos++] = '\"'; position++; continue;
			case 'n': buffer[bufpos++] = '\n'; position++; continue;
			case 't': buffer[bufpos++] = '\t'; position++; continue;
			}
			buffer[bufpos++] = '\\';
		}
		if (source[position] == '\"')
			break;
		buffer[bufpos++] = source[position++];
	}
	position++;
	addToken(ttSTRING, buffer);
}
static void tokenizeNumber() {
	// перепписати ту фігню тре
	int num_type = numINT;
	char buffer[VAL_SIZE] = "";
	int bufpos = 0;
	int pc = 0;
	while (isAvailableNumber(source[position])) {
		if (source[position] == '.') { // чекаєм чи дійсне число
			if (pc > 0) { // якщо більше чим 1 то кажем: Каліцькие Число (насправді той хто писав таку прогу каліка)
				writeError(erNULL, "invalid FloatNumber");
			}
			pc++;
			num_type = numFLOAT;
		}
		buffer[bufpos++] = source[position++];
	}
	switch (num_type) {
		case numINT:
			addToken(ttNUMBER, buffer);
			break;
		case numFLOAT:
			addToken(ttFLOATNUM, buffer);
			break;
	}
}
static void tokenizeHex() {
	char buffer[VAL_SIZE] = "";
	int bufpos = 0;
	position += 2;
	while (isAvailableHexNumber(source[position])) {
		buffer[bufpos++] = source[position++];
	}
	addToken(ttHEXNUM, buffer);
}
static TokenType getTokenByChar(const char c) {
	int idx = indexOf(OPERATION_CHARS, c);
	if (idx == -1)
		return ttNULL;
	return OPERATION_TOKEN[idx];
}
static TokenType getTokenByWord(const char *s) {
	for (int i = 0; i < (sizeof(WORD_STRINGS) / sizeof(*WORD_STRINGS));i++) {
		if (!strcmp(WORD_STRINGS[i], s)) {
			return WORD_TOKEN[i];
		}
	}
	return ttNULL;
}

// tools

static bool isAvailableName(char c) {
	if (c >= 'a' && c <= 'z')
		return true;
	if (c >= 'A' && c <= 'Z')
		return true;
	if (c >= '0' && c <= '9')
		return true;
	if (c == '_')
		return true;
	return false;
}
static bool isAvailableNumber(char c) {
	if (c >= '0' && c <= '9')
		return true;
	if (c == '.')
		return true;
	return false;
}
static bool isAvailableHexNumber(char c) {
	if (c >= '0' && c <= '9')
		return true;
	if (c >= 'A' && c <= 'F')
		return true;
	if (c >= 'a' && c <= 'f')
		return true;
	if (c == '.')
		writeError(erNULL, "invalid HexNumber");
	return false;
}
static int indexOf(const char *s, const char c) {
	const char *p = strchr(s, c);
	if (p) {
		return p - s;
	}
	return -1;
}
