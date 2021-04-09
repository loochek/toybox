section .text

global _Z14dict_calc_hashPKc

_Z14dict_calc_hashPKc:
    xor eax, eax

    mov cl, [rdi]
    test cl, cl
    jz end

loop:
    movzx esi, cl
    add eax, esi
    mov esi, eax
    shl esi, 10
    add eax, esi
    mov esi, eax
    shr esi, 6
    xor eax, esi
    
    inc rdi
    mov cl, [rdi]
    test cl, cl
    jnz loop

end:
    mov esi, eax
    shl esi, 3
    add eax, esi
    mov esi, eax
    shr esi, 11
    xor eax, esi
    mov esi, eax
    shl esi, 15
    add eax, esi

    ret
