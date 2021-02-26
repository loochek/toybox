AS=nasm
ASFLAGS=-f elf
LD=ld
LDFLAGS=-s -m elf_i386

printf_test: start.o printf.o utils.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f *.o printf_test