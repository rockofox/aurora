#include <stdint.h>
#include <stddef.h>

char *strcat(char *dest, const char *src)
{
	char *ret = dest;
	while (*dest)
		dest++;
	while (*dest++ = *src++)
		;
	return ret;
}
int strcmp(char string1[], char string2[])
{
	for (int i = 0;; i++)
	{
		if (string1[i] != string2[i])
		{
			return string1[i] < string2[i] ? -1 : 1;
		}

		if (string1[i] == '\0')
		{
			return 0;
		}
	}
}
void strcpy(char dest[], const char source[])
{
	int i = 0;
	while ((dest[i] = source[i]) != '\0')
	{
		i++;
	}
}

/**
	 * C++ version 0.4 char* style "itoa":
	 * Written by Lukás Chmela
	 * Released under GPLv3.

	 */
char *itoa(uint32_t value, char *result, int base)
{
	// check that the base if valid
	if (base < 2 || base > 36)
	{
		*result = '\0';
		return result;
	}

	char *ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do
	{
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
	} while (value);

	// Apply negative sign
	if (tmp_value < 0)
		*ptr++ = '-';
	*ptr-- = '\0';
	while (ptr1 < ptr)
	{
		tmp_char = *ptr;
		*ptr-- = *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
int strtol(const char *str, char **endptr, int base)
{
	int ret = 0;
	int sign = 1;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	while (*str)
	{
		if (*str >= '0' && *str <= '9')
		{
			ret *= base;
			ret += *str - '0';
		}
		else if (*str >= 'a' && *str <= 'z')
		{
			ret *= base;
			ret += *str - 'a' + 10;
		}
		else if (*str >= 'A' && *str <= 'Z')
		{
			ret *= base;
			ret += *str - 'A' + 10;
		}
		else
		{
			break;
		}
		str++;
	}
	if (endptr)
		*endptr = (char *)str;
	return sign * ret;
}