VRAM_ADDR     equ 0B800h
SCREEN_WIDTH  equ 80
SCREEN_HEIGHT equ 25

;-------------------------------------------
; prints a block of the text (respecting \n)
; si - address of $-terminated string
; ch - color attribute
; bl - collumn offset
; bh - row offset
;-------------------------------------------
; TRASHES: ax, bx, dx, si, es
;-------------------------------------------
print_text proc
	mov dl, bl             ; remember collumn offset for new-line'ing

print_text_loop:
	cmp [si], byte ptr '$'
	je print_text_loop_end

	cmp [si], byte ptr 0Ah ; '\n'
	je print_text_new_line

	mov cl, [si]

	save <bx, dx>
	call put_char		   ; TRASHES: ax, bx, dx, es
	rest <dx, bx>

	inc bl
	jmp print_text_loop_final

print_text_new_line:
	inc bh
	mov bl, dl
	
print_text_loop_final:
	inc si
	jmp print_text_loop

print_text_loop_end:
	ret

print_text endp


;-------------------------------------------
; cl - ascii code (use NUL to write color only)
; ch - color attribute
; bl - collumn offset
; bh - row offset
;-------------------------------------------
; TRASHES: ax, bx, dx, es
;-------------------------------------------
put_char proc
	cmp bl, SCREEN_WIDTH
	jae put_char_exit

	cmp bh, SCREEN_HEIGHT
	jae put_char_exit

	mov ax, SCREEN_WIDTH * 2
	mov dl, bh
	xor dh, dh
	mul dx

	mov bh, 00h
	add bx, bx
	add ax, bx
	
	mov bx, VRAM_ADDR
	mov es, bx
	mov bx, ax
	
	cmp cl, 00h
	je skip_char_write
	mov es:[bx], cl

skip_char_write:
	mov es:[bx + 1], ch

put_char_exit:
	ret

put_char endp
