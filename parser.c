#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "error.h"
#include "types.h"
#include "tokens.h"
#include "expression.h"
#include "statement.h"

//static TokenType t;
//static char value[64];

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
static Expression* operand();
static Expression* function(char*);
static Expression* element();
static Expression* word();

static char *variableGet();
static int matchVariable();

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
	return 0;
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
	// getTokenAt(0).type == ttVARIABLE
	int pos = matchVariable();
	if (pos > 0 && getTokenWithout(pos, ttEOL).type == ttOPENBLOCK) { // TODO chek is variable
		char *var = variableGet();
		//tokenMatch(ttVARIABLE);
		tokenSkip(ttEOL);
		tokenMatch(ttOPENBLOCK);
		return TableStatement(var);
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
		char *var = variableGet();
		return DeleteStatement(var); // TODO create fucntion Record* getVar()
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
	int pos = matchVariable();
	if (pos > 0) {
		if (getTokenAt(pos).type == ttASSIGN) { //start parsing of assign statement
			char *var = variableGet();
			tokenMatch(ttASSIGN);
			return AssignStatement(var, expression());
		}
		else if (getTokenAt(pos).type == ttSQOPEN) {
			char *var = variableGet();
			expression_node_t *indices = NULL;
			do {
				tokenMatch(ttSQOPEN);
				expression_push(&indices, expression()); // collect indices
				if (!tokenMatch(ttSQCLOSE))
					writeError(erEXPECTATION, "]");
			} while(getToken().type == ttSQOPEN);
			if (!tokenMatch(ttASSIGN))
				writeError(erEXPECTATION, "=");
			return ArrayAssignStatement(var, indices, expression());
		}
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
	char *var = variableGet();
	tokenMatch(ttVARIABLE);
	tokenMatch(ttPOPEN);
	char args[256] = "";
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
		return FuncDefStatement(var, args, st);
	}
	else {
		return FuncDefStatement(var, args, NULL); // if func haven't body
	}
}
// function
static Expression *function(char *name) { // fucntion expression
	expression_node_t *args = NULL;
	int counter = 0;
	tokenMatch(ttPOPEN);
	while (!tokenMatch(ttPCLOSE)) {
		expression_push(&args, expression());
		if (tokenMatch(ttCOMMA)) { // (6,) and match token comma
			if (getToken().type == ttPCLOSE) // то не змінна то вираз
				writeError(erEXPECTATION, "<EXPRESSION> (argument)");
		}
		counter++;
	}
	if (counter > 0)
		return functionExpression(name, args);
	else {
		free(args);
		return functionExpression(name, NULL);
	}
}
// element
static Expression *element(char *name) {
	expression_node_t *indices = NULL;
	do {
		tokenMatch(ttSQOPEN);
		expression_push(&indices, expression()); // collect indices
		if (!tokenMatch(ttSQCLOSE))
			writeError(erEXPECTATION, "]");
	} while(getToken().type == ttSQOPEN);
	return elementExpression(name, indices);
}
// expressions
static Expression* expression() {
	return conditional();
}
// conditional (logic)
static Expression* conditional() {
	Expression *result = addition();
	for (;;) {
		// tokenMatch2 to front cuz have 2 tokens
		if (tokenMatch2(ttASSIGN, ttASSIGN)) {
			result = conditionalExpression(opEQ, result, addition());
			continue;
		}
		if (tokenMatch2(ttNOT, ttASSIGN)) {
			result = conditionalExpression(opNEQ, result, addition());
			continue;
		}
		if (tokenMatch2(ttGT, ttASSIGN)) {
			result = conditionalExpression(opGREQ, result, addition());
			continue;
		}
		if (tokenMatch2(ttLT, ttASSIGN)) {
			result = conditionalExpression(opLSEQ, result, addition());
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
	if (tokenMatch(ttNUMBER)) { // integer
		return integerExpression((int)strtol(current.value, NULL, 10));
	}
	if (tokenMatch(ttHEXNUM)) { // integer
		return integerExpression((int)strtol(current.value, NULL, 16));
	}
	if (tokenMatch(ttFLOATNUM)) { // number
		return floatExpression(atof(current.value));
	}
	if (tokenMatch(ttSTRING)) { // string
		return stringExpression(current.value);
	}
	if (tokenMatch(ttTRUE)) { // boolean
		return boolExpression(true);
	}
	if (tokenMatch(ttFALSE)) { // boolean
		return boolExpression(false);
	}
	if (tokenMatch(ttNIL)) { // nil
		return nilExpression();
	}
	if (matchVariable() > 0) { // variable // getTokenAt(0).type == ttVARIABLE
		return word();
	}
	if (tokenMatch(ttSQOPEN)) { // array definition
		tokenSkip(ttEOL);
		expression_node_t *elements = NULL;
		while (!tokenMatch(ttSQCLOSE)) {
			if (getToken().type == ttCOMMA)
				writeError(erEXPECTATION, "[element]");
			expression_push(&elements, expression());
			if (tokenMatch(ttCOMMA)) {
				if (getTokenWithout(0, ttEOL).type == ttSQCLOSE)
					writeError(erEXPECTATION, "[element]");
			}
			else {
				if (getTokenWithout(0, ttEOL).type != ttSQCLOSE)
					writeError(erEXPECTATION, ",");
			}
			tokenSkip(ttEOL);
		}
		return listExpression(elements);
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
static Expression* word() {
	char *var = variableGet();
	// parse suffix
	if (getToken().type == ttPOPEN) { // if function tokenMatch(ttPOPEN)
		return function(var);
	}
	else if (getToken().type == ttSQOPEN) { // if list tokenMatch(ttSQOPEN)
		return element(var);
	}
	else { // if variable
		return variableExpression(var);
	}
	return NULL;
}
static char *variableGet() {
	/**
	* @return variable string ex."table.var"
	*/
	Token current = getToken();
	if (!tokenMatch(ttVARIABLE)) // match variable
		writeError(erEXPECTATION, "<Variable>");
	char *var = malloc(sizeof(char) * 256);
	strcat(var, current.value);
	while (tokenMatch(ttDOT)) { // if dot token, then record is table
		strcat(var, ".");
		current = getToken();
		if (!tokenMatch(ttVARIABLE)) // match variable
			writeError(erEXPECTATION, "<Table.Variable>");
		strcat(var, current.value);
	}
	return var;
}
static int matchVariable() {
	/**
	* @return token 0 - if doesn't match, length of variable tokens
	*/
	int i = 0;
	if (getTokenAt(i).type == ttVARIABLE) { // table.subtable
		i++;
		while (getTokenAt(i).type == ttDOT && getTokenAt(i + 1).type == ttVARIABLE) {
			i += 2;
		}
		if (getTokenAt(i).type == ttDOT) // table.
		 	writeError(erEXPECTATION, "<Table.Variable>");
		return i;
	}
	return i;
}
