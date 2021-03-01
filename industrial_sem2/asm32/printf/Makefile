CC=clang
CFLAGS=-m32 -Wall

AS=nasm
ASFLAGS=-f elf -w+all

LD=ld
LDFLAGS=-s -m elf_i386

printf_test: printf_test_start.o printf.o utils.o
	$(LD) $(LDFLAGS) $^ -o $@

c_test: c_main.o c_test_start.o printf.o utils.o
	$(LD) $(LDFLAGS) $^ -o $@

call_test: call_test_start.o c_sum.o printf.o utils.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -f *.o printf_test c_test call_test