TOOLCHAIN=i386-elf-11.2.0-Linux-x86_64
export PATH := $(shell pwd)/build/$(TOOLCHAIN)/bin:$(PATH)

SRCS = $(shell find -name '*.c' -or -name '*.asm')
OBJS = $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)
-include $(DEPS)

CC = i386-elf-gcc
LD = i386-elf-ld


ASFLAGS = -m32
CFLAGS = -m32 -std=gnu11 -ffreestanding -Wall -Wextra -fno-stack-protector -fno-builtin -isystem src/std -g
# LDFLAGS = -m32 -T linker.ld -ffreestanding -O2 -nostdlib -fno-stack-protector
LDFLAGS = -melf_i386 -T linker.ld 
VPATH=obj

aurora.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
run: aurora.bin
	qemu-system-i386 -device VGA -accel tcg,thread=single -cpu core2duo -smp 1 -m 128 -kernel aurora.bin -serial stdio
debug: aurora.bin
	qemu-system-i386 -accel tcg,thread=single -cpu core2duo -smp 1 -m 128 -kernel aurora.bin -serial stdio -S -gdb tcp::1234 -d int -no-reboot -no-shutdown
all: aurora.bin
aurora.iso: aurora.bin
	# mkdir -p iso/boot/grub
	cp aurora.bin iso/boot
	# if [ ! -f iso/boot/grub/grub.cfg ]; then
	# 	echo 'set timeout=5' >> iso/boot/grub/grub.cfg
	# 	echo 'set default=0' >> iso/boot/grub/grub.cfg
	# 	echo 'menuentry "Aurora" {' >> iso/boot/grub/grub.cfg
	# 	echo 'multiboot /boot/aurora.bin' >> iso/boot/grub/grub.cfg
	# 	echo '}' >> iso/boot/grub/grub.cfg
	# fi
	grub-mkrescue -o aurora.iso iso
isodebug: aurora.iso
	qemu-system-i386 -cdrom aurora.iso -serial stdio -S -gdb tcp::1234 -d cpu_reset -no-reboot -no-shutdown -drive format=raw,file=test.img -boot d
isorun: aurora.iso
	 qemu-system-i386 -cdrom aurora.iso -serial stdio -accel kvm -smp 1 -m 128 -vga std -machine pc -drive format=raw,if=ide,index=0,file=test.img -boot d
isoverbose: aurora.iso
	 qemu-system-i386 -cdrom aurora.iso -serial stdio -accel kvm -smp 1 -m 128 -vga std -d int,cpu_reset
toolchain:
	mkdir -p build
	curl -o build/$(TOOLCHAIN).tar.xz http://newos.org/toolchains/$(TOOLCHAIN).tar.xz
	tar -C build/ -xvJpf build/$(TOOLCHAIN).tar.xz
	rm -f build/$(TOOLCHAIN).tar.xz
%.o: %.c
	$(CC) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.asm
	nasm -M -MF $(patsubst %.o,%.d,$@) $^
	nasm -f elf $^ -g -o $@


.PHONY: clean
clean:
	rm $(OBJS) $(DEPS)
