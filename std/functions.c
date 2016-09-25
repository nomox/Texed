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
		    printf("nil");
				break;
		}
	}
	return valueNil();
}
