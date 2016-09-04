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
TokenType getType() {
	return tokens[current_position].type;
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
	if (type != getType())
		return false;
	current_position++;
	return true;
}

void printList() {
	for (int i = 0; i < count_position;i++) {
		printf("%d:%s\n", tokens[i].type, tokens[i].value);
	}
}
