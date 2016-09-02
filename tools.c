#include <stdbool.h>

int newInt(int size)
{
	return (int)malloc(size * sizeof(int));
}
/*
float newFloat(int size)
{
	return (float)malloc(size * sizeof(float));
}
*/
char newChar(int size)
{
	return (char)malloc(size * sizeof(char));
}
char *newString(int size)
{
	return (char*)malloc(size * sizeof(char*));
}
void insertChar(char *str, int pos, char ch) {
	int l = strlen(str);
	for (int i = l + 1; i > pos; i--) 
		str[i] = str[i - 1];
	str[pos] = ch;
}
void strclear(char *str, int size) {
	for (int i = 0; i < size; i++) str[i] = 0;
	//str[0] = '\0';
}
bool isAvailableName(char c) {
	if (c >= 'a' && c <= 'z')
		return true;
	if (c >= 'A' && c <= 'Z')
		return true;
	if (c >= '0' && c <= '9')
		return true;
	if (c == '_')
		return true;
	return false;
}
bool isAvailableNumber(char c) {
	if (c >= '0' && c <= '9')
		return true;
	if (c == '.')
		return true;
	return false;
}
int indexOf(const char *s, const char c) {
	const char *p = strchr(s, c);
	if (p) {
		return p - s;
	}
	return -1;
}