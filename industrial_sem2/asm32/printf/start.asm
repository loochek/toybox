section .text

global _start

extern _printf

_start:
    mov eax, 0xFFFFFFFF

    push example_str
    push 65
    push eax
    push eax
    push eax
    push eax
    push example_fmt
    call _printf
    add esp, 4 * 7

    mov eax, 0x01
    mov ebx, 0
    int 0x80


;-----------------------------------------------
section .data
example_fmt   db `Example format string\n`
              db `Dec: %d\n`
              db `Bin: %b\n`
              db `Oct: %o\n`
              db `Hex: %x\n`
              db `Single char: %c\n`
              db `String: %s\n`
              db 0x00

example_str   db `test string`, 0x00