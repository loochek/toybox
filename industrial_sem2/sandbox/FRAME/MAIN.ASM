.model tiny
.code

;------------------------------
save macro regs
	irp r, <regs>
		push r
	endm
endm

rest macro regs
	irp r, <regs>
		pop r
	endm
endm
;------------------------------

org 0100h

MAIN_COLOR equ 70h
ARGC_ADDR  equ 80h
ARGS_ADDR  equ 82h

;-------------------------------------------
; main
;-------------------------------------------
start proc

	mov dx, 2989 ; BAD
	mov di, offset frame_text
	call fill_buffer

    mov di, offset frame_header

    mov bx, ARGC_ADDR
    cmp [bx], byte ptr 0                    ; is command line argument present?
    je start_custom_header_skip

    mov di, ARGS_ADDR

start_custom_header_skip:
	mov bl, 20
	mov bh, 4
	mov al, 40
	mov ah, 8
	mov si, offset frame_text
	mov ch, MAIN_COLOR
	call msg_box
	
	ret

frame_header  db "Important message", '$'
frame_text    db 512 dup(0)                 ; reserve memory for "formatted string"

start endp


;-------------------------------------------
; fills buffer at [di]
; formats dx with different bases
;-------------------------------------------
; TRASHES: ax, bx, cx, si, di
; Restores ES to CS
;-------------------------------------------
fill_buffer proc
	mov cx, cs
	mov es, cx

	mov cx, fill_buffer_str1_len
	mov si, offset fill_buffer_str1
	rep movsb

	mov cl, 1
	save<dx>
	call itoa_pow2
	rest<dx>

	mov cx, fill_buffer_str2_len
	mov si, offset fill_buffer_str2
	rep movsb

	mov cl, 3
	save<dx>
	call itoa_pow2
	rest<dx>

	mov cx, fill_buffer_str3_len
	mov si, offset fill_buffer_str3
	rep movsb

	mov cl, 4
	save<dx>
	call itoa_pow2
	rest<dx>

	mov cx, fill_buffer_str4_len
	mov si, offset fill_buffer_str4
	rep movsb

	mov cx, 10
	save<dx>
	call itoa
	rest<dx>

	mov al, '$'
	stosb

	ret

fill_buffer_str1     db "Value of DX:", 0Ah, "Bin: "
fill_buffer_str1_len equ $ - offset fill_buffer_str1

fill_buffer_str2     db 0Ah, "Oct: "
fill_buffer_str2_len equ $ - offset fill_buffer_str2

fill_buffer_str3     db 0Ah, "Hex: "
fill_buffer_str3_len equ $ - offset fill_buffer_str3

fill_buffer_str4     db 0Ah, "Dec: "
fill_buffer_str4_len equ $ - offset fill_buffer_str4

fill_buffer endp


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

include WINDOW.ASM
end start
