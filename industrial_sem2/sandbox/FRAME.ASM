.model tiny
.186
.code

VRAM_ADDR     equ 0B800h
SCREEN_WIDTH  equ 80
SCREEN_HEIGHT equ 25

MAIN_COLOR    equ 70h
SHADOW_COLOR  equ 08h

SHADOW_VERTICAL_OFFSET   equ 1
SHADOW_HORIZONTAL_OFFSET equ 2

MSG_BOX_TEXT_VERTICAL_OFFSET   equ 1
MSG_BOX_TEXT_HORIZONTAL_OFFSET equ 2

org 0100h

start:
	mov bl, 20
	mov bh, 4
	mov al, 40
	mov ah, 7
	mov si, offset example_header
	mov di, offset example_message
	mov ch, MAIN_COLOR
	call msg_box
	
	ret

; bl - column offset
; bh - row offset
; al - width
; ah - height
; si - address of header string
; di - address of text string
; ch - color attribute
msg_box:
	pusha
	push bx

	call show_frame
	
	call str_len
	mov dh, al
	sub dh, dl
	sar dh, 1
	add bl, dh
	call print_text

	pop bx  ; restore trashed offsets
	add bl, MSG_BOX_TEXT_HORIZONTAL_OFFSET
	add bh, MSG_BOX_TEXT_VERTICAL_OFFSET
	mov si, di
	call print_text
	
	popa
	ret

example_header  db "Important message", '$'
example_message db "Multi", 0Ah, "Line", 0Ah, "Test", 0Ah, "String!", '$'


; display frame with animation
; bl - column offset
; bh - row offset
; al - width
; ah - height
; ch - color attribute
show_frame:
	pusha
	
	mov dl, 02h
	mov dh, 02h

show_frame_horizontal_expand_loop:
	call draw_frame
	call delay
	inc dl
	
	cmp dl, al
	jb show_frame_horizontal_expand_loop
	
show_frame_vertical_expand_loop:
	call draw_frame
	call delay
	inc dh
	
	cmp dh, ah
	jb show_frame_vertical_expand_loop	
	
	popa
	ret
	

; bl - collumn offset
; bh - row offset
; dl - width
; dh - height
; ch - color attribute
draw_frame:
	pusha

	mov cl, 'É'
	mov al, 'Í'
	mov ah, '»'
	call draw_line
	call draw_line_shadow
	
	mov cl, 'º'
	mov al, ' '
	mov ah, 'º'
	dec dh		; to correct frame height

draw_frame_loop:
	inc bh
	call draw_line
	call draw_line_shadow
	
	dec dh
	jnz draw_frame_loop
	
	mov cl, 'È'
	mov al, 'Í'
	mov ah, '¼'
	call draw_line
	call draw_line_shadow
	
	popa
	ret	

	
; bl - column offset
; bh - row offset
; dl - width
; cl, al, ah - ascii codes of first, middle and last characrters
; ch - color attribute
draw_line:
	pusha
	call put_char
	
	mov cl, al
	dec dl		; to correct line width

draw_line_loop:
	inc bl
	call put_char
	
	dec dl
	jnz draw_line_loop
	
	mov cl, ah
	call put_char

	popa
	ret
	
	
; use this after draw_line to draw shadow for line
draw_line_shadow:
	pusha

	mov ch, SHADOW_COLOR
	mov cl, 00h
	mov al, 00h
	mov ah, 00h

	add bl, SHADOW_HORIZONTAL_OFFSET
	add bh, SHADOW_VERTICAL_OFFSET
	
	call draw_line
	
	popa
	ret


; print a block of the text (respecting \n)
; si - address of $-terminated string
; ch - color attribute
; bl - collumn offset
; bh - row offset
print_text:
	pusha
	
	mov dl, bl ; remember collumn offset for new-line'ing

print_text_loop:
	cmp [si], byte ptr '$'
	je print_text_loop_end

	cmp [si], byte ptr 0Ah ; '\n'
	je print_text_new_line

	mov cl, [si]
	call put_char
	inc bl
	jmp print_text_loop_final

print_text_new_line:
	inc bh
	mov bl, dl
	
print_text_loop_final:
	inc si
	jmp print_text_loop

print_text_loop_end:
	popa
	ret

	
; cl - ascii code (use NUL to write color only)
; ch - color attribute
; bl - collumn offset
; bh - row offset
put_char:
	pusha

	cmp bl, SCREEN_WIDTH
	jae put_char_exit

	cmp bh, SCREEN_HEIGHT
	jae put_char_exit

	mov ax, SCREEN_WIDTH * 2
	mov dl, bh
	mov dh, 00h
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
	popa
	ret

	
; si - address of $-terminated string
; result in dl
str_len:
	push si
	mov dl, 00h
	
str_len_loop:
	cmp [si], byte ptr '$'
	je str_len_loop_end

	inc dl
	inc si
	jmp str_len_loop
	
str_len_loop_end:
	pop si
	ret


; animation delay
; sleep for 25ms
delay:
	pusha

	mov dx, 61A8h
	mov cx, 0000h
	mov ah, 86h
	int 15h

	popa
	ret	

end start
