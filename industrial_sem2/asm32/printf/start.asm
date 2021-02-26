section .text

global _start

extern _printf

_start:
    push 0xFFFFFFFF
    push 0xFFFFFFFF
    push 0xFFFFFFFF
    push 0xFFFFFFFF
    push ex_str
    push msg
    call _printf
    add esp, 4 * 6

    mov eax, 0x01
    mov ebx, 0
    int 0x80


section .data
msg           db "%s", 0x0A, "Dec: %d", 0x0A, "Bin: %b", 0x0A, "Oct: %o", 0x0A, "Hex: %x", 0x0A, 0x00
ex_str        db "Test string", 0x00