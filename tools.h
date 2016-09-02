#pragma once

int newInt(int size);
float newFloat(int size);
double newDouble(int size);
char newChar(int size);
char *newString(int size);
void insertChar(char *str, int pos, char ch);
void strclear(char *str, int size);
bool isAvailableName(char c);
bool isAvailableNumber(char c);
int indexOf(const char *s, const char c);