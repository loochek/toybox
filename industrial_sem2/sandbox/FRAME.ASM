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

start proc
	mov bl, 20
	mov bh, 4
	mov al, 40
	mov ah, 7
	mov di, offset example_header
	mov si, offset example_message
	mov ch, MAIN_COLOR
	call msg_box
	
	ret

start endp

example_header  db "Important message", '$'
example_message db "Multi", 0Ah, "Line", 0Ah, "Test", 0Ah, "String!", '$'


;-------------------------------------------
; bl - column offset
; bh - row offset
; al - width
; ah - height
; di - address of header string
; si - address of text string
; ch - color attribute
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, si, di, es
;-------------------------------------------
msg_box proc
	call show_frame                               ; TRASHES: dx, es

	save <bx, cx, ax, di>
	mov cx, cs
	mov es, cx
	call str_len                                  ; TRASHES: al, di, result in cx
	rest <di>
	xchg si, di

	rest <ax>
	mov dh, al
	sub dh, cl
	sar dh, 1
	add bl, dh
	rest <cx>
	call print_text                               ; TRASHES: ax, bx, dx, si, es

	rest <bx>
	add bl, MSG_BOX_TEXT_HORIZONTAL_OFFSET
	add bh, MSG_BOX_TEXT_VERTICAL_OFFSET
	mov si, di
	call print_text                               ; TRASHES: ax, bx, dx, si, es

	ret

msg_box endp


;-------------------------------------------
; display frame with animation
; bl - column offset
; bh - row offset
; al - width
; ah - height
; ch - color attribute
;-------------------------------------------
; TRASHES: dx, es
;-------------------------------------------
show_frame proc
	mov dl, 02h
	mov dh, 02h

show_frame_horizontal_expand_loop:
	save <ax, bx, cx, dx>
	call draw_frame                            ; TRASHES: ax, bx, cx, dx, es
	rest <dx, cx, bx, ax>

	save <ax, cx, dx>
	call delay                                 ; TRASHES: ah, cx, dx
	rest <dx, cx, ax>

	inc dl
	cmp dl, al
	jb show_frame_horizontal_expand_loop
	
show_frame_vertical_expand_loop:
	save <ax, bx, cx, dx>
	call draw_frame                            ; TRASHES: ax, bx, cx, dx, es
	rest <dx, cx, bx, ax>

	save <ax, cx, dx>
	call delay                                 ; TRASHES: ah, cx, dx
	rest <dx, cx, ax>

	inc dh
	cmp dh, ah
	jb show_frame_vertical_expand_loop	
	
	ret
	
show_frame endp

;-------------------------------------------
; bl - collumn offset
; bh - row offset
; dl - width
; dh - height
; ch - color attribute
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, es
;-------------------------------------------
draw_frame proc
	mov cl, 'É'
	mov al, 'Í'
	mov ah, '»'

	save <bx, cx, dx>
	call draw_line           ; TRASHES: ax, bl, cl, dl, es
	rest <dx, cx, bx>

	save <bx, cx, dx>
	call draw_line_shadow    ; TRASHES: ax, bx, cx, dl, es
	rest <dx, cx, bx>
	
	mov cl, 'º'
	mov al, ' '
	mov ah, 'º'

	dec dh		             ; to correct frame height

draw_frame_loop:
	inc bh

	save <ax, bx, cx, dx>
	call draw_line           ; TRASHES: ax, bl, cl, dl, es
	rest <dx, cx, bx, ax>

	save <ax, bx, cx, dx>
	call draw_line_shadow    ; TRASHES: ax, bx, cx, dl, es
	rest <dx, cx, bx, ax>
	
	dec dh
	jnz draw_frame_loop
	
	mov cl, 'È'
	mov al, 'Í'
	mov ah, '¼'

	save <bx, cx, dx>
	call draw_line            ; TRASHES: ax, bl, cl, dl, es
	rest <dx, cx, bx>

	call draw_line_shadow     ; TRASHES: ax, bx, cx, dl, es
	
	ret	

draw_frame endp


;-------------------------------------------
; bl - column offset
; bh - row offset
; dl - width
; cl, al, ah - ascii codes of first, middle and last characrters
; ch - color attribute
;-------------------------------------------
; TRASHES: ax, bl, cl, dl, es
;-------------------------------------------
draw_line proc
	save <ax, bx, dx>
	call put_char      ; TRASHES: ax, bx, dx, es
	rest <dx, bx, ax>
	
	mov cl, al
	dec dl		       ; to correct line width

draw_line_loop:
	inc bl

	save <ax, bx, dx>
	call put_char      ; TRASHES: ax, bx, dx, es
	rest <dx, bx, ax>
	
	dec dl
	jnz draw_line_loop
	
	mov cl, ah
	call put_char      ; TRASHES: ax, bx, dx, es

	ret

draw_line endp
	

;-------------------------------------------
; use this after draw_line to draw shadow for line
; bl - column offset
; bh - row offset
; dl - width
;-------------------------------------------
; TRASHES: ax, bx, cx, dl, es
;-------------------------------------------
draw_line_shadow proc
	mov ch, SHADOW_COLOR
	mov cl, 00h
	mov al, 00h
	mov ah, 00h

	add bl, SHADOW_HORIZONTAL_OFFSET
	add bh, SHADOW_VERTICAL_OFFSET
	
	call draw_line                        ; TRASHES: ax, bl, cl, dl, es
	
	ret

draw_line_shadow endp


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

end start