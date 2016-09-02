#include "variables.h"
#include "types.h"

TokenType t;
char value[64];
int pos;
int parse() {
	do {
		pos = getNextToken(&t, value);

	} while (pos != -1);
}
int parseLine() {

}
