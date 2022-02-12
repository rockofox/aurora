#include "io.h"
#include "serial.h"
#include "string.h"
#include <stdarg.h>

void init_serial()
{
    outb(PORT + 1, 0x00); // Disable all interrupts
    outb(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00); //                  (hi byte)
    outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}
int is_transmit_empty()
{
    return inb(PORT + 5) & 0x20;
}
void write_serial(char a)
{
    while (is_transmit_empty() == 0)
        ;

    outb(PORT, a);
}
void serial_print(char *str)
{
    while (*(str++) != '\0')
    {
        write_serial(*(str - 1));
    }
}
void serial_println(char *str)
{
    serial_print(str);
    serial_print("\n");
}
void serial_printf(char *str, ...)
{
    char *arg;
    int i = 0;
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
                arg = va_arg(args, int);
                itoa(arg, n, 10);
                serial_print(n);
                break;
            case 'x':
                arg = va_arg(args, int);
                itoa(arg, n, 16);
                serial_print(n);
                break;
            case 's':
                arg = va_arg(args, char *);
                serial_print(arg);
                break;
            case 'c':
                arg = va_arg(args, char);
                serial_putc(arg);
                break;
            default:
                break;
            }
        }
        else
        {
            write_serial(*str);
        }
        str++;
    }
    va_end(args);
}