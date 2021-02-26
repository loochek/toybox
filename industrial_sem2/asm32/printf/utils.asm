section .text

global itoa, itoa_pow2

;-------------------------------------------
; converts unsigned integer from edx to string with base ecx (2 <= ecx <= 36)
; writes result to [edi] and moves edi to byte after number
; $0x________  --->  $0x12345___
;	 ^edi                    ^edi
;-------------------------------------------
; TRASHES: eax, ebx, ecx, edx, esi
;-------------------------------------------
itoa:
	mov esi, edi
	mov edi, itoa_buffer
    
	cmp ecx, 36
	ja itoa_error_base

    mov eax, edx

itoa_loop:
	xor edx, edx
	div ecx
	
    mov ebx, edx
	mov ch, [itoa_digits + ebx]
	mov [edi], ch
    dec edi
    xor ch, ch
	
	test eax, eax
	jnz itoa_loop

itoa_error_base:
	xchg esi, edi
	inc esi
	mov ecx, itoa_buffer + 1
	sub ecx, esi
	rep movsb

	ret


;-------------------------------------------
; converts unsigned long long from edx
; to string with base 2^cl (2 <= cl <= 5)
; writes result to [edi] and moves edi to byte after number
; $0x________  --->  $0x12345___
;	 ^edi                    ^edi
; more efficient than itoa
;-------------------------------------------
; TRASHES: eax, ebx, cx, edx, esi
;-------------------------------------------
itoa_pow2:
    mov ebx, itoa_digits
    mov esi, edi
    mov edi, itoa_buffer
    std

    cmp cl, 5
	ja itoa_pow2_error_base

    mov ch, 0FFh
    shl ch, cl
    not ch

itoa_pow2_loop:
    mov eax, edx
    and al, ch
    shr edx, cl

    xlatb
    stosb

    test edx, edx
    jnz itoa_pow2_loop

itoa_pow2_error_base:
    cld
    xchg esi, edi
    inc esi
	mov ecx, itoa_buffer + 1
	sub ecx, esi
	rep movsb

    ret


;------------------------------------------------------
section .data
			  times 31 db 0
itoa_buffer   db 0           ; 32 digits in worst case
itoa_digits   db "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"