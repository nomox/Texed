#include "types.h"
#include "tokens.h"
#include "error.h"

#define NULL 0
#define VAL_SIZE 64

int count_position = 0, current_position = 0;
Token *tokens = NULL;
Token *tokentmp;

Token *newToken(Token *memory, int size) {
	return  (Token*)realloc(memory, size*sizeof(Token));
}

int addToken(TokenType type, const char *value) {
	tokentmp = newToken(tokens, count_position + 1);

	if (tokentmp != NULL) {
		tokens = tokentmp;
		tokens[count_position].ttype = type; // assign token type
		tokens[count_position].tvalue = (char*)malloc(VAL_SIZE * sizeof(char*)); // value (string) !NEED UPDATE TO UNION
		strcpy(tokens[count_position].tvalue, value);
	}
	else {
		free(tokens);
		writeError(erMEMORYREALLOCATION, "");
		return -1;
	}

	return count_position++;
}
int getToken(int position, TokenType *type, char *value) {
	if (position < count_position) {
		*type = tokens[position].ttype;
		strcpy(value, tokens[position].tvalue);
		//printf("val = %s\n", tokens[position].tvalue);
		return position;
	}
	else {
		//writeError(NULL, "No such token");
		return -1;
	}
}
int getNextToken(TokenType *type, char *value) {
	return getToken(current_position++, type, value);
}
