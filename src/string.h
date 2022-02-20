#pragma once
#include <stddef.h>

char *strcat(char *dest, const char *src);
int strcmp(char string1[], char string2[]);
void strcpy(char dest[], const char source[]);
char *itoa(uint32_t value, char *result, int base);
int strtol(char *str, char **endptr, int base);
size_t strlen(const char *str);