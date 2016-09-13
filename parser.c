#include "stdlib.h"
#include "stdbool.h"
#include "types.h"
#include "tokens.h"
#include "expression.h"
#include "statement.h"
#include "error.h"

static TokenType t;
static char value[64];
static int pos;

static Statement *statement();
static Statement *block();
static Statement *statementPreferred();
static Statement *assignmentStatement();
static Statement *conditionalStatement();
static Statement *whileStatement();

static Expression* expression();
static Expression* conditional();
static Expression* addition();
static Expression* multiply();
static Expression* unary();
static Expression* operand();

int parse() {
	//Expression *e;
	//e = expression();
	//printf("%d\n", e->get(e));
	while (getToken().type != ttEOF) {
		Statement *s;
		s = statement();
		if (s != NULL)
			s->execute(s);
		free(s); // чисттимо
		if (!tokenMatch(ttEOL) && getToken().type != ttEOF) {
			writeError(erEXPECTATION, "<EOL>");
		}
	}
}

//statements
static Statement *block() {
	Statement *bloc_st = BlockStatement();
	StatementBlock *st = bloc_st->statement;

	if (!tokenMatch(ttOPENBLOCK))
		writeError(erEXPECTATION, "{");
	while (!tokenMatch(ttCLOSEBLOCK)) {
		statements_push(st->list, statement());
		//free(s); // чисттимо
		if (getToken().type == ttEOF)
			writeError(erEXPECTATION, "}");
		if (!tokenMatch(ttEOL) && getToken().type != ttCLOSEBLOCK) {
			writeError(erEXPECTATION, "<EOL>");
		}
	}
	return bloc_st;
}
static Statement *statementPreferred() {
	if (getToken().type == ttOPENBLOCK)
		return block();
	else
		return statement();
}
static Statement *statement() {
	tokenSkip(ttEOL);
	if (tokenMatch(ttIF)) {
		return conditionalStatement();
	}
	if (tokenMatch(ttWHILE)) {
		return whileStatement();
	}
	if (tokenMatch(ttPRINT)) {
		return PrintStatement(expression());
	}
	if (tokenMatch(ttERROR)) {
		return ErrorStatement(expression());
	}
	if (tokenMatch(ttDELETE)) {
		Token current = getToken();
		if (tokenMatch(ttVARIABLE))
			return DeleteStatement(current.value);
		else
			writeError(erEXPECTATION, "<Variable>");
	}
	return assignmentStatement();
}
static Statement *assignmentStatement() {
	Token current = getToken();
	if (tokenMatch(ttVARIABLE) && tokenMatch(ttASSIGN)) {
		return AssignStatement(current.value, expression());
	}
	else {
		if (getToken().type == ttEOF)
			return NULL;
		//printf("tt %d\n", getToken().type);
		writeError(erEXPECTATION, "<STATEMENT>");
	}
}
static Statement *conditionalStatement() {
	Expression *expr = expression();
	tokenSkip(ttEOL);
	Statement *ifStatement = statementPreferred();
	Statement *elseStatement;
	if (getTokenAt(1).type == ttELSE) tokenSkip(ttEOL);
	if (tokenMatch(ttELSE)) {
		elseStatement = statementPreferred();
	}
	else {
		elseStatement = NULL;
	}
	return ConditionStatement(expr, ifStatement, elseStatement);
}
static Statement *whileStatement() {
	Expression *expr = expression();
	tokenSkip(ttEOL);
	Statement *whStatement = statementPreferred();
	return WhileStatement(expr, whStatement);
}
// expressions
static Expression* expression() {
	return conditional();
}
// conditional (logic)
static Expression* conditional() {
	Expression *result = addition();
	for (;;) {
		if (tokenMatch(ttASSIGN)) {
			result = conditionalExpression(opEQ, result, addition());
			continue;
		}
		if (tokenMatch(ttGT)) {
			result = conditionalExpression(opGRTH, result, addition());
			continue;
		}
		if (tokenMatch(ttLT)) {
			result = conditionalExpression(opLSTH, result, addition());
			continue;
		}
		break;
	}
	return result;
}
// binary operations addition, subtaction, multilication, divide
static Expression* addition() {
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
static Expression* multiply() {
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
static Expression* unary() {
	if (tokenMatch(ttMINUS)) {
		return unaryExpression(opNEGNUM, operand());
	}
	return operand();
}
static Expression* operand() { // numbers and strings // високий приорітет
	Token current = getToken();
	//printf("kek %d '%s'\n", current.type, current.value);
	if (tokenMatch(ttNUMBER)) {
		return integerExpression((int)strtol(current.value, NULL, 10));
	}
	if (tokenMatch(ttHEXNUM)) {
		return integerExpression((int)strtol(current.value, NULL, 16));
	}
	if (tokenMatch(ttSTRING)) {
		return stringExpression(current.value);
	}
	if (tokenMatch(ttTRUE)) {
		return boolExpression(true);
	}
	if (tokenMatch(ttFALSE)) {
		return boolExpression(false);
	}
	if (tokenMatch(ttNIL)) {
		return nilExpression();
	}
	if (tokenMatch(ttVARIABLE)) {
		return variableExpression(current.value);
	}
	if (tokenMatch(ttPOPEN)) {
		Expression *result = expression();
		if (!tokenMatch(ttPCLOSE))
			writeError(erEXPECTATION ,")");
		return result;
	}

	writeError(erEXPECTATION, "<EXPRESSION>");
	return NULL;
}
