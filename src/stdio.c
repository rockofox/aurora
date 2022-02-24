#include "terminal.h"
#include "unistd.h"
#include <stdarg.h>
void kprintln(const char *data)
{
	scrollback_write(data);

	terminal_row++;
	terminal_column = 0;
	while (terminal_row > VGA_HEIGHT + scrollback_getposition())
	{
		terminal_scrolldown(VGA_HEIGHT);
	}
}
void kprint(const char *data)
{
	scrollback_write(data);
}
void kputc(char c)
{
	scrollback_putc(c);
}
void kprintf(char *str, ...)
{
	char *arg;
	uint32_t i = 0;
	char n[10];
	va_list args;
	va_start(args, str);
	while (*str != '\0')
	{
		if (*str == '%')
		{
			str++;
			switch (*str)
			{
			case 'd':
				arg = va_arg(args, uint32_t);
				itoa(arg, n, 10);
				kprint(n);
				break;
			case 'x':
				arg = va_arg(args, uint32_t);
				itoa(arg, n, 16);
				kprint(n);
				break;
			case 's':
				arg = va_arg(args, char *);
				kprint(arg);
				break;
			case 'c':
				arg = va_arg(args, char);
				kputc(arg);
				break;
			default:
				break;
			}
		}
		else
		{
			kputc(*str);
		}
		str++;
	}
	va_end(args);
}