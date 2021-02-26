section .text

global _printf

extern itoa, itoa_pow2

;----------------------------------------------
; (cdecl)
; int printf(const char *fmt, ...)
; prints formatted string
; it's a usual C string, but some
; sequences are interpreted differently:
; %d - put decimal number given as argument
;      instead of this sequence
;      Example:
;          printf("It costs %d bucks", 10)
;          // prints "It costs 10 bucks"
;
; Other sequences work the same way
; %b, %o, %x - put number in corresponding base
; %s         - put C string
; %c         - put char
;----------------------------------------------
_printf:
    push ebp
    mov ebp, esp

    push ebx
    push esi
    push edi

    mov esi, [ebp+8]                           ; address of fmt string
    mov edi, printf_buffer

    mov ebx, ebp
    add ebx, 8 + 4                             ; address of current formatting argument

printf_fmt_loop:
    cmp [esi], byte 0x00
    je printf_fmt_loop_end

    cmp [esi], byte '%'
    je printf_percent_handler

    movsb
    jmp printf_fmt_loop
    
printf_fmt_loop_end:
    mov eax, 0x04                               ; write
    mov ebx, 0x01                               ; stdout
    mov ecx, printf_buffer
    mov edx, edi
    sub edx, printf_buffer
    int 0x80

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret

printf_percent_handler:
    inc esi
    cmp [esi], byte 's'
    je printf_string_handler

    cmp [esi], byte 'x'
    je printf_hex_handler
    
    cmp [esi], byte 'o'
    je printf_oct_handler

    cmp [esi], byte 'b'
    je printf_bin_handler

    cmp [esi], byte 'd'
    je printf_dec_handler


printf_string_handler:
    push esi
    mov esi, [ebx]
    add ebx, 4
    call put_string
    pop esi

    inc esi
    jmp printf_fmt_loop

; TODO: replace copy-paste with macro

printf_hex_handler:
    mov edx, [ebx]
    add ebx, 4
    mov cl, 4

    push esi
    push ebx
    call itoa_pow2
    pop ebx
    pop esi

    inc esi
    jmp printf_fmt_loop

printf_oct_handler:
    mov edx, [ebx]
    add ebx, 4
    mov cl, 3

    push esi
    push ebx
    call itoa_pow2
    pop ebx
    pop esi

    inc esi
    jmp printf_fmt_loop

printf_bin_handler:
    mov edx, [ebx]
    add ebx, 4
    mov cl, 1

    push esi
    push ebx
    call itoa_pow2
    pop ebx
    pop esi

    inc esi
    jmp printf_fmt_loop

printf_dec_handler:
    mov edx, [ebx]
    add ebx, 4
    mov ecx, 10

    push esi
    push ebx
    call itoa
    pop ebx
    pop esi

    inc esi
    jmp printf_fmt_loop


;-----------------------------------------------
; moves C string from esi to edi
; edi is moved after string
; \0 is not moved
;--------
; cats\0|  I love aaaaaa --> I love catsaa
; ^esi  |         ^edi                  ^edi
;       |
;-----------------------------------------------
; TRASHES: esi
;-----------------------------------------------
put_string:
    cmp [esi], byte 0x00
    je put_string_end
    movsb
    jmp put_string
    
put_string_end:
    ret


;-----------------------------------------------
section .data
printf_buffer times 500 db 0