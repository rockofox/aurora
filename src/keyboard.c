#include <stdbool.h>
#include "keyboard.h"
#include "terminal.h"
#include "stdio.h"
#include "io.h"
char layout[60] = "1234567890-=<>qwertyuiop[]?\\asdfghjkl;'!@#zxcvbnm,./";
char buffer[200] = "";
bool shiftKey = false;
bool ctrlKey = false;
enum KeyState keyMatrix[64] = {false};
enum KeyState prevMatrix[64] = {false};
void execute(char* cmd) {
	if(strcmp("test",cmd) == 0) {
		kprint("it works!");
	}
	else if(strcmp("time",cmd) == 0) {
		while(1) {
			
		}
	}
	else if(strcmp("int",cmd) == 0) {
		asm volatile("int $0x0");
	}
	else if(strcmp("shutdown", cmd) == 0) {
		// Initiate a shutdown, trying methods for several platform
		outw(0x604, 0x2000); // QEMU
		outw(0xB004, 0x2000); // Bochs, old QEMU
		outw(0x4004, 0x3400); // Virtualbox
		shutdown(); // Try to shutdown via realmode
	}
	else if(strcmp("cls", cmd) == 0) {
		terminal_clear();
	}
	else if(strcmp("l", cmd) == 0) {
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("sed diam nonumy eirmod tempor invidunt ut labore et d");
	}
	else {
		char msg[50] = "Unknown command ";
		strcat(msg, cmd);
		kprintln(msg);
	}
}


void irq1_handler(void) {
		uint8_t key = inb(0x60);
		if((key >> 7) > 0) {
			keyMatrix[(key & 0x7F) - 0x02] = Released;
		}
		else {
			keyMatrix[(key & 0x7F) - 0x02] = Pressed;
		}
		if(key == 0x2a) shiftKey = true;
		if(key == 0xaa) shiftKey = false;

		if(key == 0x1d) ctrlKey = true;
		if(key == 0x9d) ctrlKey = false;

		for (size_t i = 0; i < 60; i++)
		{
            if (layout[i] == 's' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i] && ctrlKey) {
				terminal_scrolldown(1);
				// break;
			}
			if (layout[i] == 'w' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i] && ctrlKey)
			{
				terminal_scrollup(1);
				// break;
			}
			if (layout[i] == '<' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i])
			{
                if(terminal_column > strlen(PROMPT)) {
                    buffer[strlen(buffer)-1] = 0;
                    terminal_column--;
                    kprint(" ");
                    terminal_column--;
                }
			}
			if (layout[i] == '?' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i])
			{
				terminal_row++;
				terminal_column=0;
				execute(buffer);
				*buffer = 0;
				kprintln("");
				kprint(PROMPT);
			}
			if (keyMatrix[i] != prevMatrix[i] && keyMatrix[i] == Pressed && layout[i] != '@' && layout[i] != '?' && layout[i] != '<' && !ctrlKey)
			{
				char c = layout[i];
				if(shiftKey) c&=0xdf;
				kputc(c);
				append(buffer, c);
			}
		}
		for (size_t i = 0; i < 60; i++) prevMatrix[i] = keyMatrix[i];
	  outb(0x20, 0x20); //EOI
}