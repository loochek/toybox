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
;
; Return value is the number of characters printed
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
    int 0x80                                    ; return value in eax

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret


printf_percent_handler:
    inc esi
    ; parse format character according to jump table in .data section
    movzx edx, byte [esi]
    jmp [printf_jump_table + 4 * edx]


; entries for jump table:

printf_bad_fmt_arg:
    ; just skip in case of bad formatting argument
    inc esi
    jmp printf_fmt_loop


printf_double_percent_handler:
    ; put second percent to result string
    movsb
    jmp printf_fmt_loop


printf_char_handler:
    mov eax, [ebx]
    add ebx, 4

    stosb

    inc esi
    jmp printf_fmt_loop


printf_string_handler:
    push esi
    mov esi, [ebx]
    add ebx, 4

put_string:
    cmp [esi], byte 0x00
    je put_string_end
    movsb
    jmp put_string

put_string_end:
    pop esi
    inc esi
    jmp printf_fmt_loop


%macro num_handler 2
%1:
    mov edx, [ebx]
    add ebx, 4
    mov cl, %2

    push esi
    push ebx
    call itoa_pow2
    pop ebx
    pop esi

    inc esi
    jmp printf_fmt_loop
%endmacro

num_handler printf_bin_handler, 1
num_handler printf_oct_handler, 3
num_handler printf_hex_handler, 4


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
section .data
printf_buffer times 512 db 0  ; overflooow.....

; format characters jump table
; looks weird, but better than conditional jumps
;
; description:
; ...37chars...%...59chars..._bcd__________o___s____x__...133chars...
; |(fmt chars) ^              ^^^          ^   ^    ^               |
; |--------------------------256 possible chars---------------------|
printf_jump_table times 37 dd printf_bad_fmt_arg
                           ; percent - ascii code 37
                           dd printf_double_percent_handler
                  times 59 dd printf_bad_fmt_arg
                           ; lowercase ascii letters - starting from 97
                           dd printf_bad_fmt_arg
                           dd printf_bin_handler
                           dd printf_char_handler
                           dd printf_dec_handler
                  times 10 dd printf_bad_fmt_arg
                           dd printf_oct_handler
                   times 3 dd printf_bad_fmt_arg
                           dd printf_string_handler
                   times 4 dd printf_bad_fmt_arg
                           dd printf_hex_handler
                   times 2 dd printf_bad_fmt_arg
                           ; the rest of byte values
                 times 133 dd printf_bad_fmt_arg