; calling C function from asm

section .text

global _start

extern sum, _printf

_start:
    push 300
    push -100
    call sum
    add esp, 4 * 2
    
    push eax    ; sum result
    push fmt_string
    call _printf
    add esp, 4 * 1

    mov eax, 0x01
    mov ebx, 0
    int 0x80


;-----------------------------------------------
section .data
fmt_string: db `%d\n`, 0x00