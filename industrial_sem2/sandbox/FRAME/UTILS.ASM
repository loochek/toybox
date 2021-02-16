;-------------------------------------------
; es:di - address of $-terminated string
; result in cx
;-------------------------------------------
; TRASHES: al, di
;-------------------------------------------
str_len proc
	mov al, '$'
	xor cx, cx
	dec cx
	cld
	repne scasb
	neg cx
	sub cx, 2

	ret

str_len endp


;-------------------------------------------
; animation delay
; sleep for 25ms
;-------------------------------------------
; TRASHES: ah, cx, dx
;-------------------------------------------
delay proc
	mov dx, 61A8h
	xor cx, cx
	mov ah, 86h
	int 15h

	ret

delay endp


;-------------------------------------------
; converts unsigned integer from dx to string with base cx (2 <= cx <= 36)
; writes result to [di] and moves di to byte after number
; $0x________  --->  $0x12345___
;	 ^di                     ^di
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, si
; Restores ES to CS
;-------------------------------------------
itoa proc
    mov bx, cs
	mov es, bx

	mov si, di
	mov di, offset itoa_buffer
    
	cmp cx, 36
	ja itoa_error_base

    mov ax, dx

itoa_loop:
	xor dx, dx
	div cx
	
    mov bx, dx
	mov ch, [offset itoa_digits + bx]
	mov [di], ch
    dec di
    xor ch, ch
	
	test ax, ax
	jnz itoa_loop

itoa_error_base:
	xchg si, di
	inc si
	mov cx, offset itoa_buffer + 1
	sub cx, si
	rep movsb

	ret
	
itoa endp


;-------------------------------------------
; converts unsigned integer from dx to string with base 2^cl (2 <= cl <= 5)
; writes result to [di] and moves di to byte after number
; $0x________  --->  $0x12345___
;	 ^di                     ^di
; more efficient than itoa
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, si
; Restores ES to CS
;-------------------------------------------
itoa_pow2 proc
    mov bx, cs
    mov es, bx

    mov bx, offset itoa_digits
    mov si, di
    mov di, offset itoa_buffer
    std

    cmp cl, 5
	ja itoa_pow2_error_base

    mov ch, 0FFh
    shl ch, cl
    not ch

itoa_pow2_loop:
    mov ax, dx
    and al, ch
    shr dx, cl

    xlatb
    stosb

    test dx, dx
    jnz itoa_pow2_loop

itoa_pow2_error_base:
    cld
    xchg si, di
    inc si
	mov cx, offset itoa_buffer + 1
	sub cx, si
	rep movsb

    ret

itoa_pow2 endp

			db 15 dup(0)
itoa_buffer db 0           ; 16 digits in worst case
itoa_digits db "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"