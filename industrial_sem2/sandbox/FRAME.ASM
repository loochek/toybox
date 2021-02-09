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

org 0100h

start:
	; display sample frame
	mov bl, 20
	mov bh, 4
	mov al, 40
	mov ah, 10
	call show_frame
	
	add bl, 2
	add bh, 2
	mov ch, MAIN_COLOR
	mov si, offset example_string
	call print_string
	
	ret

example_string db "Test string", '$'

; display frame with animation
; bl - column offset
; bh - row offset
; al - width
; ah - height
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
draw_frame:
	pusha

	mov ch, MAIN_COLOR	
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


; si - address of $-terminated string
; ch - color attribute
; bl - collumn offset
; bh - row offset
print_string:
	pusha
	
print_string_loop:
	cmp [si], byte ptr '$'
	je print_string_loop_end
	
	mov cl, [si]
	call put_char
	
	inc bl
	inc si
	jmp print_string_loop

print_string_loop_end:
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
