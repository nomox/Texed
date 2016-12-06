#include "stdio.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "../error.h"
#include "functions.h"

expression_value_t *_print(expression_value_t *args[], int arg_length) {
	for (int i = 0; i < arg_length; i++) {
		switch (args[i]->type) {
			case dtINTEGER:
		    printf("%d", args[i]->value.i);
				break;
			case dtFLOAT:
			  printf("%g", args[i]->value.f);
				break;
		  case dtSTRING:
		    printf(args[i]->value.s);
				break;
		  case dtBOOLEAN:
		    printf((args[i]->value.b)?"true":"false");
				break;
		  case dtNIL:
		    printf("<nil>");
				break;
			case dtTABLE:
			  printf("<table>");
				break;
			default:
				return valueNil();
		}
	}
	return valueNil();
}
expression_value_t *_read(expression_value_t *args[], int arg_length) {
	char str[256];
  scanf("%s", str);
	return valueString(str);
}
expression_value_t *_listsize(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtLIST)
		writeError(erEXPECTATION, "<list> (as 1 argument)");
	return valueInteger(list_size(args[0]->value.list));
}
expression_value_t *_listcreate(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtINTEGER)
		writeError(erEXPECTATION, "<INTEGER> (as 1 argument)");
	list_node_t *list = NULL;
	for (int i = 0; i < args[0]->value.i; i++) {
		list_push(&list, valueNil());
	}
	return valueList(list);
}
expression_value_t *_listpush(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtLIST)
		writeError(erEXPECTATION, "<list> (as 1 argument)");
	list_push(&(args[0]->value.list), args[1]);
	return valueNil();
}
expression_value_t *_listremove(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtLIST)
		writeError(erEXPECTATION, "<list> (as 1 argument)");
	if (args[1]->type != dtINTEGER)
		writeError(erEXPECTATION, "<INTEGER> (as 2 argument)");
	if (args[1]->value.i >= list_size(args[0]->value.list))
		writeError(erOUTOFRANGE, "_list");
	list_remove(&(args[0]->value.list), args[1]->value.i);
	return valueNil();
}
expression_value_t *_listisempty(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtLIST)
		writeError(erEXPECTATION, "<list> (as 1 argument)");
	if (list_empty(args[0]->value.list))
		return valueBoolean(true);
	else
		return valueBoolean(false);
}
expression_value_t *_strlength(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtSTRING)
		writeError(erEXPECTATION, "<string> (as 1 argument)");
	return valueInteger(strlen(args[0]->value.s));
}
expression_value_t *_strcat(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtSTRING)
		writeError(erEXPECTATION, "<string> (as 1 argument)");
	if (args[1]->type != dtSTRING)
		writeError(erEXPECTATION, "<string> (as 2 argument)");
	return valueString(strcat(args[0]->value.s, args[1]->value.s));
}
expression_value_t *_strtoint(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtSTRING)
		writeError(erEXPECTATION, "<string> (as 1 argument)");
	return valueInteger((int)strtol(args[0]->value.s, NULL, 10));
}
expression_value_t *_strtonumber(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtSTRING)
		writeError(erEXPECTATION, "<string> (as 1 argument)");
	return valueFloat(atof(args[0]->value.s));
}
expression_value_t *_sin(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtFLOAT)
		writeError(erEXPECTATION, "<number> (as 1 argument)");
	return valueFloat(sin(args[0]->value.f));
}
expression_value_t *_cos(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtFLOAT)
		writeError(erEXPECTATION, "<number> (as 1 argument)");
	return valueFloat(cos(args[0]->value.f));
}
expression_value_t *_sqrt(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtFLOAT)
		writeError(erEXPECTATION, "<number> (as 1 argument)");
	return valueFloat(sqrt(args[0]->value.f));
}
expression_value_t *_time(expression_value_t *args[], int arg_length) {
	return valueFloat(time(NULL));
}
expression_value_t *_system(expression_value_t *args[], int arg_length) {
	if (args[0]->type != dtSTRING)
		writeError(erEXPECTATION, "<string> (as 1 argument)");
	system(args[0]->value.s);
	return valueNil();
}
