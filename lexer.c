#include "stdio.h"
#include "ctype.h"
#include "types.h"
#include "tokens.h"
#include "stdbool.h"
#include "error.h"

#define LN_SIZE 512
#define VAL_SIZE 64
#define VAL_EMPTY ""


void tokenizeLine(char*);
void tokenizeWord();
void tokenizeString();
void tokenizeNumber();

const char OPERATION_CHARS[] = "+-*/:.,()[]{}#=&$%@!?";
const TokenType OPERATION_TOKEN[] = {
	ttPLUS,
	ttMINUS,
	ttMULTIPLY,
	ttDIVIDE,
	ttEXDENT,
	ttDOT,
	ttCOMMA,
	ttBOPEN,
	ttBCLOSE,
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
	ttISTRUE
};
const char *WORD_STRINGS[] = {
	"if",
	"else",
	"switch",
	"case",
	"or",
	"and",
	"for",
	"while",
	"to",
	"times",
	"do",
	"function",
	"this",
	"self",
	"use",
	"nil",
	"true",
	"false"
};
const TokenType WORD_TOKEN[] = {
	ttIF,
	ttELSE,
	ttSWITCH,
	ttCASE,
	ttOR,
	ttAND,
	ttFOR,
	ttWHILE,
	ttTO,
	ttTIMES,
	ttDO,
	ttFUNCTION,
	ttTHIS,
	ttSELF,
	ttUSE,
	ttNIL,
	ttTRUE,
	ttFALSE
};
bool skip = false;
int nline = 0;
char *source;
int position = 0;
TokenType tt;

TokenType getTokenByChar(const char c);
TokenType getTokenByWord(const char *s);

void tokenize(FILE *f) {
	char tmp_line[LN_SIZE];
	while (fgets(tmp_line, LN_SIZE, f) != NULL) {
		tokenizeLine(tmp_line);
		//printf("line: %d\n", nline);
	}
}

void tokenizeLine(char *line) {
	source = line;
	nline++;
	while (source[position] != '\0') { // vars and reserved words
		// обробка коментарів
		if (source[position] == '/' && source[position + 1] == '/') {
			break; //  якщо коментар // то пропускаєм строку
		}
		else if (source[position] == '*' && source[position + 1] == '/') {
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
	addToken(ttEOL, VAL_EMPTY); // end of line
	position = 0;
}
void tokenizeWord() {
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
void tokenizeString() {
	char buffer[VAL_SIZE] = "";
	int bufpos = 0;
	position++;
	while (source[position] != '\"') {
		buffer[bufpos++] = source[position++];
	}
	position++;
	addToken(ttSTRING, buffer);
}
void tokenizeNumber() {
	char buffer[VAL_SIZE] = "";
	int bufpos = 0;
	while (isAvailableNumber(source[position])) {
		buffer[bufpos++] = source[position++];
	}
	addToken(ttNUMBER, buffer);
}
TokenType getTokenByChar(const char c) {
	int idx = indexOf(OPERATION_CHARS, c);
	if (idx == -1)
		return ttNULL;
	return OPERATION_TOKEN[idx];
}
TokenType getTokenByWord(const char *s) {
	for (int i = 0; i < (sizeof(WORD_STRINGS) / sizeof(*WORD_STRINGS));i++) {
		if (!strcmp(WORD_STRINGS[i], s)) {
			return WORD_TOKEN[i];
		}
	}
	return ttNULL; // ���� �� �� �������� �����
}
