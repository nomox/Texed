#include "stdlib.h"
#include "stdbool.h"
#include "types.h"
#include "tokens.h"
#include "error.h"

#define VAL_SIZE 64

int count_position = 0;
int current_position = 0;
Token *tokens = NULL;
Token *tokentmp;

static TokenType getType();

Token *newToken(Token *memory, int size) {
	return  (Token*)realloc(memory, size*sizeof(Token));
}

int addToken(TokenType type, const char *value) {
	tokentmp = newToken(tokens, count_position + 1);

	if (tokentmp != NULL) {
		tokens = tokentmp;
		tokens[count_position].type = type; // assign token type
		tokens[count_position].value = (char*)malloc(VAL_SIZE * sizeof(char*)); // value (string) !NEED UPDATE TO UNION
		strcpy(tokens[count_position].value, value);
	}
	else {
		free(tokens);
		writeError(erMEMORYREALLOCATION, "");
		return -1;
	}

	return count_position++;
}
static TokenType getType() {
	/*
	if (current_position >= count_position) {
		printf("Out of size %d\n", count_position);
		exit(0);
	}*/
	return getToken().type;
}
Token getToken() {
	if (current_position < count_position) {
		return tokens[current_position];
	}
	Token eof;
	eof.type = ttEOF;
	eof.value = NULL;
	return eof;
}
Token getTokenOffset(int offset) {
	if (current_position + offset < count_position) {
		return tokens[current_position + offset];
	}
	Token eof;
	eof.type = ttEOF;
	eof.value = NULL;
	return eof;
}
Token getNextToken() {
	if (current_position < count_position) {
		return tokens[current_position++];
	}
	Token eof;
	eof.type = ttEOF;
	eof.value = NULL;
	return eof;
}
bool tokenMatch(TokenType type) {
	//while (getType() == ttEOL) current_position++; // skip
	if (type != getType())
		return false;
	current_position++;
	return true;
}
void tokenMiss(TokenType type) {
	while (tokenMatch(ttEOL));
}
int getLine() {
	int lines = 0;
	for (int i = 0; i < current_position; i++) {
		if (tokens[i].type == ttEOL) lines++;
	}
	return lines+1;
}
void printList() {
	for (int i = 0; i < count_position;i++) {
		printf("%d:%s\n", tokens[i].type, tokens[i].value);
	}
}
