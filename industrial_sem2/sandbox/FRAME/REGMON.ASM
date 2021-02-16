REGMON_MAIN_COLOR     equ 70h

REGMON_ROW_OFFSET     equ 1
REMGON_COLLUMN_OFFSET equ 1
REGMON_WINDOW_WIDTH   equ 19
REGMON_WINDOW_HEIGHT  equ 6

;-------------------------------------------
; shows Registers Monitor window with
; AX, BX, CX, DX registers values
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, si, di
; Restores ES to CS
;-------------------------------------------
regmon_show proc
	mov di, offset frame_text
	call regmon_text_fmt			        ; TRASHES: ax, bx, cx, dx, si, di

	mov bl, REMGON_COLLUMN_OFFSET
	mov bh, REGMON_ROW_OFFSET
	mov dl, REGMON_WINDOW_WIDTH
	mov dh, REGMON_WINDOW_HEIGHT
	mov di, offset frame_header
	mov si, offset frame_text
	mov ch, REGMON_MAIN_COLOR
	call msg_box						    ; TRASHES: ax, bx, cx, dx, si, di
	
	ret

frame_header  db "Registers monitor", '$'
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

	mov si, offset regmon_text_fmt_str1
	mov cx, regmon_text_fmt_str1_len
	rep movsb

	mov cl, 4
	pop dx
	call itoa_pow2		; TRASHES: ax, bx, cx, dx, si

	mov si, offset regmon_text_fmt_str2
	mov cx, regmon_text_fmt_str2_len
	rep movsb

	mov cl, 4
	pop dx
	call itoa_pow2		; TRASHES: ax, bx, cx, dx, si

	mov si, offset regmon_text_fmt_str3
	mov cx, regmon_text_fmt_str3_len
	rep movsb

	mov cl, 4
	pop dx
	call itoa_pow2		; TRASHES: ax, bx, cx, dx, si

	mov si, offset regmon_text_fmt_str4
	mov cx, regmon_text_fmt_str4_len
	rep movsb

	mov cl, 4
	pop dx
	call itoa_pow2		; TRASHES: ax, bx, cx, dx, si

	mov al, '$'
	stosb

	ret

regmon_text_fmt_str1     db  "AX: 0x"
regmon_text_fmt_str1_len equ $ - regmon_text_fmt_str1

regmon_text_fmt_str2     db  0Ah, "BX: 0x"
regmon_text_fmt_str2_len equ $ - regmon_text_fmt_str2

regmon_text_fmt_str3     db  0Ah, "CX: 0x"
regmon_text_fmt_str3_len equ $ - regmon_text_fmt_str3

regmon_text_fmt_str4     db  0Ah, "DX: 0x"
regmon_text_fmt_str4_len equ $ - regmon_text_fmt_str4

regmon_text_fmt endp

include WINDOW.ASM