#pragma once
#define PORT 0x3f8 /* COM1 */

void init_serial();
void write_serial(char a);
void serial_println(char* str);
void serial_print(char* str);