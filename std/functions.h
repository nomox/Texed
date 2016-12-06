#include "../expression.h"
#include "../memory.h"

expression_value_t *_print(expression_value_t *args[], int arg_length);
expression_value_t *_read(expression_value_t *args[], int arg_length);
expression_value_t *_listsize(expression_value_t *args[], int arg_length);
expression_value_t *_listcreate(expression_value_t *args[], int arg_length);
expression_value_t *_listpush(expression_value_t *args[], int arg_length);
expression_value_t *_listremove(expression_value_t *args[], int arg_length);
expression_value_t *_listisempty(expression_value_t *args[], int arg_length);
expression_value_t *_strlength(expression_value_t *args[], int arg_length);
expression_value_t *_strcat(expression_value_t *args[], int arg_length);
expression_value_t *_strtoint(expression_value_t *args[], int arg_length);
expression_value_t *_strtonumber(expression_value_t *args[], int arg_length);
expression_value_t *_sin(expression_value_t *args[], int arg_length);
expression_value_t *_cos(expression_value_t *args[], int arg_length);
expression_value_t *_sqrt(expression_value_t *args[], int arg_length);
expression_value_t *_time(expression_value_t *args[], int arg_length);
expression_value_t *_system(expression_value_t *args[], int arg_length);
