#pragma once
#include <stddef.h>
#include <stdint.h>

char *strcat(char *dest, const char *src);
int strcmp(char string1[], char string2[]);
void strcpy(char dest[], const char source[]);
char *itoa(uint32_t value, char *result, int base);
int strtol(char *str, char **endptr, int base);
char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);
size_t strlen(const char *str);
char *strchr(const char *str, int c);
void *memcpy(void *destination, const void *source, size_t num);
void *memset(void *destination, int value, size_t num);
int memcmp(const void *ptr1, const void *ptr2, size_t num);