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
Token getTokenOffset(int);
bool tokenMatch(TokenType);
void tokenMiss(TokenType);
int getLine();
