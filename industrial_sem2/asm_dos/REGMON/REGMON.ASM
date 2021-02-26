REGMON_MAIN_COLOR     equ 70h
REGMON_WINDOW_WIDTH   equ 14
REGMON_WINDOW_HEIGHT  equ 6
REGMON_BASE           equ 4    ; registers values are shown with (2^REGMON_BASE) base

;-------------------------------------------
; shows Registers Monitor window with
; AX, BX, CX, DX registers values
; LSB of si - collumn offset
; MSB of si - row offset
;
; di - address of $-term. string
;    - frame header
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, si, di
; Restores ES to CS
;-------------------------------------------
regmon_show proc
	push si di
	mov di, offset frame_text
	call regmon_text_fmt			        ; TRASHES: ax, bx, cx, dx, si, di
	pop  di si

	mov bx, si
	mov dl, REGMON_WINDOW_WIDTH
	mov dh, REGMON_WINDOW_HEIGHT
	mov si, offset frame_text
	mov ch, REGMON_MAIN_COLOR
	call msg_box						    ; TRASHES: ax, bx, cx, dx, si, di
	
	ret

frame_text    db 512 dup(0)                 ; reserve memory for "formatted string"

regmon_show endp


;-------------------------------------------
; formats AX, BX, CX, DX values as text:
; AX: 0xXXXXXXXX
; ....
; stores result string at [di]
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, si, di
; Restores ES to CS
;-------------------------------------------
regmon_text_fmt proc
	push dx cx bx ax

	mov cx, cs
	mov es, cx

irpc x, 1234
	mov si, offset regmon_text_fmt_str&x
	mov cx, regmon_text_fmt_str_len&x
	rep movsb

	mov cl, REGMON_BASE
	pop dx
	call itoa_pow2		; TRASHES: ax, bx, cx, dx, si
endm

	mov al, '$'
	stosb

	ret

regmon_text_fmt_str1     db  "AX: 0x"
regmon_text_fmt_str_len1 equ $ - regmon_text_fmt_str1

regmon_text_fmt_str2     db  0Ah, "BX: 0x"
regmon_text_fmt_str_len2 equ $ - regmon_text_fmt_str2

regmon_text_fmt_str3     db  0Ah, "CX: 0x"
regmon_text_fmt_str_len3 equ $ - regmon_text_fmt_str3

regmon_text_fmt_str4     db  0Ah, "DX: 0x"
regmon_text_fmt_str_len4 equ $ - regmon_text_fmt_str4

regmon_text_fmt endp

include WINDOW.ASM