#pragma once

typedef struct TOKEN {
	TokenType ttype;
	char *tvalue;
} Token;

int addToken(TokenType type, const char *value);
int getToken(int position, TokenType *type, char *value);
int getNextToken(TokenType *type, char *value);
