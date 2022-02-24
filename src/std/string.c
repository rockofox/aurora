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
// strchr implementation
char *strchr(const char *str, int c)
{
	while (*str)
	{
		if (*str == c)
			return (char *)str;
		str++;
	}
	return NULL;
}
void *memcpy(void *destination, const void *source, size_t num)
{
	uint8_t *dst = (uint8_t *)destination;
	uint8_t *src = (uint8_t *)source;
	for (size_t i = 0; i < num; i++)
	{
		dst[i] = src[i];
	}
	return destination;
}
void *memset(void *destination, int value, size_t num)
{
	long *d = destination;
	for (size_t i = 0; i < num; i++)
	{
		d[i] = value;
	}
	return destination;
}
int memcmp(const void *ptr1, const void *ptr2, size_t num)
{
	uint8_t *p1 = (uint8_t *)ptr1;
	uint8_t *p2 = (uint8_t *)ptr2;
	for (size_t i = 0; i < num; i++)
	{
		if (p1[i] != p2[i])
			return p1[i] - p2[i];
	}
	return 0;
}
char *strtok(char *str, const char *delim)
{
	static char *saved_str;
	if (str)
	{
		saved_str = str;
	}
	else if (!saved_str)
	{
		return NULL;
	}
	str = saved_str;
	while (*str && strchr(delim, *str))
	{
		str++;
	}
	if (!*str)
	{
		saved_str = NULL;
		return NULL;
	}
	saved_str = str + 1;
	while (*str && !strchr(delim, *str))
	{
		str++;
	}
	if (*str)
	{
		*str = '\0';
		str++;
	}
	return saved_str;
}
int strcspn(const char *str, const char *delim)
{
	int ret = 0;
	while (*str)
	{
		if (strchr(delim, *str))
			return ret;
		str++;
		ret++;
	}
	return ret;
}
int strspn(const char *str, const char *delim)
{
	int ret = 0;
	while (*str)
	{
		if (!strchr(delim, *str))
			return ret;
		str++;
		ret++;
	}
	return ret;
}
char *strtok_r(char *s, const char *delim, char **save_ptr)
{
	char *end;
	if (s == NULL)
		s = *save_ptr;
	if (*s == '\0')
	{
		*save_ptr = s;
		return NULL;
	}
	/* Scan leading delimiters.  */
	s += strspn(s, delim);
	if (*s == '\0')
	{
		*save_ptr = s;
		return NULL;
	}
	/* Find the end of the token.  */
	end = s + strcspn(s, delim);
	if (*end == '\0')
	{
		*save_ptr = end;
		return s;
	}
	/* Terminate the token and make *SAVE_PTR point past it.  */
	*end = '\0';
	*save_ptr = end + 1;
	return s;
}