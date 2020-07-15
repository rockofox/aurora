#pragma once
#include <stddef.h>

char *strcat(char *dest, const char *src);
int strcmp(char string1[], char string2[]);
void strcpy(char dest[], const char source[]);
char *itoa(int value, char *result, int base);
size_t strlen(const char* str);