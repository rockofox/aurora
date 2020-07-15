SRCS = $(shell find -name '*.c' -or -name '*.asm')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = i686-elf-gcc
LD = i686-elf-gcc

ASFLAGS = -m32
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc

aurora.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
run: aurora.bin
	qemu-system-i386 -kernel aurora.bin -vga std -display sdl -serial stdio
%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@ 

%.o: %.asm
	nasm -felf32 $^ -o $@

clean:
	rm $(OBJS)

.PHONY: clean