CC=clang
CFLAGS=-m32

AS=nasm
ASFLAGS=-f elf

LD=ld
LDFLAGS=-s -m elf_i386

printf_test: start.o printf.o utils.o
	$(LD) $(LDFLAGS) $^ -o $@

c_test: c_main.o c_start.o printf.o utils.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f *.o printf_test c_test