#include "stdlib.h"
#include "variables.h"
#include "types.h"
#include "tokens.h"
#include "expression.h"
#include "error.h"

TokenType t;
char value[64];
int pos;

Expression* addition();
Expression* multiply();
Expression* unary();
Expression* operand();
Expression* expression();

int parse() {
	Expression *e;
	e = expression();
	printf("%d\n", e->get(e));

	/*
	Expression expr;
	expr.result.i = 1488;
	printf("%d\n", expr.result.i);
	Token tmp;
	do {
		tmp = getToken();
		printf("%d\n", tmp.type);
	}
	while (tmp.type != ttEOF);
	*/
}

Expression* expression() {
	return addition();
}
// binary operations addition, subtaction, multilication, divide
Expression* addition() {
	Expression *result = multiply();
	for (;;) {
		if (tokenMatch(ttPLUS)) {
			result = binaryExpression(opADD, result, multiply());
			continue;
		}
		if (tokenMatch(ttMINUS)) {
			result = binaryExpression(opSUB, result, multiply());
			continue;
		}
		break;
	}
	return result;
}
Expression* multiply() {
	Expression *result = unary();
	for (;;) {
		if (tokenMatch(ttMULTIPLY)) {
			result = binaryExpression(opMUL, result, unary());
			continue;
		}
		if (tokenMatch(ttDIVIDE)) {
			result = binaryExpression(opDIV, result, unary());
			continue;
		}
		break;
	}
	return result;
}
// unary operations
Expression* unary() {
	if (tokenMatch(ttMINUS)) {
		return unaryExpression(opNEGNUM, operand());
	}
	return operand();
}
Expression* operand() { // numbers and strings // висовкий приорітет
	Token current = getToken();
	if (tokenMatch(ttNUMBER)) {
		return numberExpression((int)strtol(current.value, NULL, 10));
	}
	if (tokenMatch(ttHEXNUM)) {
		return numberExpression((int)strtol(current.value, NULL, 16));
	}
	if (tokenMatch(ttPOPEN)) {
		Expression *result = expression();
		tokenMatch(ttPCLOSE); // miss token
		return result;
	}

	writeError(erEXPECTATION, "EXPRESSION");
	return NULL;
}
