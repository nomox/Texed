#include "stdlib.h"
#include "stdbool.h"
#include "error.h"
#include "types.h"
#include "tokens.h"
#include "expression.h"
#include "statement.h"

static TokenType t;
static char value[64];
static int pos;

static Statement *statement();
static Statement *block();
static Statement *statementPreferred();
static Statement *assignmentStatement();
static Statement *conditionalStatement();
static Statement *whileStatement();
static Statement *funcDefine();
static Statement *defaultStatement();

static Expression* expression();
static Expression* conditional();
static Expression* addition();
static Expression* multiply();
static Expression* unary();
static Expression* suffix();
static Expression* operand();
static Expression* function();

int parse() {
	Statement *s;
	while (getToken().type != ttEOF) {
		//tokenSkip(ttEOL);
		s = statement();
		if (s != NULL)
			s->execute(s);
		//free(s); // чисттимо
		if (!tokenMatch(ttEOL) && getToken().type != ttEOF) {
			writeError(erEXPECTATION, "<EOL>");
		}
		tokenSkip(ttEOL); // skip EOLs
	}
}

//statements
static Statement *block() {
	Statement *bloc_st = BlockStatement();
	StatementBlock *st = bloc_st->statement;
	tokenSkip(ttEOL); // func() \n {}
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
static Expression *function() { // fucntion expression
	Token current = getToken();
	tokenMatch(ttVARIABLE);
	tokenMatch(ttPOPEN);
	expression_node_t *args = expression_init();
	int counter = 0;
	while (!tokenMatch(ttPCLOSE)) {
		expression_push(args, expression());
		if (tokenMatch(ttCOMMA)) { // (6,) and match token comma
			if (getToken().type == ttPCLOSE) // то не змінна то вираз
				writeError(erEXPECTATION, "<EXPRESSION> (argument)");
		}
		counter++;
	}
	if (counter > 0)
		return functionExpression(current.value, args);
	else {
		free(args);
		return functionExpression(current.value, NULL);
	}
}
static Statement *statement() {
	tokenSkip(ttEOL);
	if (getTokenAt(0).type == ttVARIABLE && getTokenWithout(1, ttEOL).type == ttOPENBLOCK) {
		Token current = getToken();
		tokenMatch(ttVARIABLE);
		tokenSkip(ttEOL);
		tokenMatch(ttOPENBLOCK);
		return TableStatement(current.value);
	}
	if (tokenMatch(ttCLOSEBLOCK)) { // close of table block
		return CloseTableStatement();
	}
	if (tokenMatch(ttIF)) {
		return conditionalStatement();
	}
	if (tokenMatch(ttWHILE)) {
		return whileStatement();
	}
	if (tokenMatch(ttFNDEF)) {
		return funcDefine();
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
	if (tokenMatch(ttBREAK)) {
		return BreakStatement();
	}
	if (tokenMatch(ttCONTINUE)) {
		return ContinueStatement();
	}
	if (tokenMatch(ttRETURN)) {
		if (getToken().type != ttEOL && getToken().type != ttCLOSEBLOCK)
			return ReturnStatement(expression());
		else
			return ReturnStatement(NULL);
	}
	return assignmentStatement();
}
static Statement *assignmentStatement() {
	Token current = getToken();
	if ((getTokenAt(0).type == ttVARIABLE && getTokenAt(1).type == ttASSIGN)) {
		tokenMatch(ttVARIABLE);
		tokenMatch(ttASSIGN);
		return AssignStatement(current.value, expression());
	}
	return defaultStatement();
}
static Statement *defaultStatement() {
	Expression *expr = expression();
	if (tokenMatch(ttISTRUE)) {
		return IfTrueStatement(expr, statementPreferred());
	}
	if (tokenMatch(ttNOT)) {
		return IfFalseStatement(expr, statementPreferred());
	}
	return DefaultStatement(expr);
	/*
	else {
		if (getToken().type == ttEOF) // якщо кінцеь файлу нічого не вертаєм
			return NULL;
		writeError(erEXPECTATION, "<STATEMENT>");
	}*/
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
static Statement *funcDefine() {
	Token current = getToken();
	tokenMatch(ttVARIABLE);
	tokenMatch(ttPOPEN);
	char args[128] = "";
	while (!tokenMatch(ttPCLOSE)) {
		Token var = getToken();
		if (tokenMatch(ttVARIABLE)) {
			strcat(args, var.value);
		}
		else {
			writeError(erEXPECTATION, "<Variable>");
		}
		if (tokenMatch(ttCOMMA)) { // (6,) and match token comma
			strcat(args, ";");
			if (getToken().type == ttPCLOSE) // то не змінна то вираз
				writeError(erEXPECTATION, "<EXPRESSION> (argument)");
		}
	}
	if (getTokenWithout(0, ttEOL).type == ttOPENBLOCK) {
		Statement *st = block();
		return FuncDefStatement(current.value, args, st);
	}
	else {
		return FuncDefStatement(current.value, args, NULL); // if func haven't body
	}
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
	return suffix();
}
static Expression* suffix() {
	Expression *expr = operand();
	storeCurrentHandler();
	while (tokenMatch(ttDOT)) { // if match dot, create sub variable expression
		expr = suffixExpression(expr, operand());
	}
	return expr;
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
	if (tokenMatch(ttFLOATNUM)) {
		return floatExpression(atof(current.value));
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
	if (getTokenAt(0).type == ttVARIABLE && getTokenAt(1).type == ttPOPEN) {
		return function();
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
