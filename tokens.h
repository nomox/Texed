#pragma once
#include "types.h"
#include "stdbool.h"

typedef struct TOKEN {
	TokenType type;
	char *value;
} Token;

int addToken(TokenType type, const char *value);
// int getToken(int position, TokenType *type, char *value);
// int getNextToken(TokenType *type, char *value);
Token getNextToken();
Token getToken();
Token getTokenAt(int);
Token getTokenWithout(int, TokenType);
bool tokenMatch(TokenType);
bool tokenMatch2(TokenType type1, TokenType type2);
void tokenSkip(TokenType);
int getLine();
