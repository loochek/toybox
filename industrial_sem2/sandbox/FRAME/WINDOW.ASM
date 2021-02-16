include SCREEN.ASM
include UTILS.ASM

SHADOW_COLOR                   equ 08h
SHADOW_VERTICAL_OFFSET         equ 1
SHADOW_HORIZONTAL_OFFSET       equ 2
MSG_BOX_TEXT_VERTICAL_OFFSET   equ 1
MSG_BOX_TEXT_HORIZONTAL_OFFSET equ 2

;-------------------------------------------
; display message box with centered header
; and text inside
; (display instantly)
;
; bl - column offset
; bh - row offset
; dl - width
; dh - height
; di - address of header string
; si - address of text string
; ch - color attribute
;-------------------------------------------
; TRASHES: ax, bx, cx, dx, si, di
; Restores ES to CS
;-------------------------------------------
msg_box proc
	push bx cx dx bx di
	call draw_frame                               ; TRASHES: ax, bx, cx, dx, es

	mov cx, cs
	mov es, cx
	call str_len                                  ; TRASHES: al, di, result in cx
	pop di
	xchg si, di

	pop bx dx
	mov ah, dl
	sub ah, cl
	sar ah, 1
	add bl, ah
	pop cx
	call print_text                               ; TRASHES: ax, bx, dx, si, es

	pop bx
	add bl, MSG_BOX_TEXT_HORIZONTAL_OFFSET
	add bh, MSG_BOX_TEXT_VERTICAL_OFFSET
	mov si, di
	call print_text                               ; TRASHES: ax, bx, dx, si, es

	ret

msg_box endp


; ;-------------------------------------------
; ; display message box with centered header
; ; and text inside
; ; (display with animation)
; ;
; ; bl - column offset
; ; bh - row offset
; ; al - width
; ; ah - height
; ; di - address of header string
; ; si - address of text string
; ; ch - color attribute
; ;-------------------------------------------
; ; TRASHES: ax, bx, cx, dx, si, di, es
; ;-------------------------------------------
; msg_box_anim proc
; 	call show_frame                               ; TRASHES: dx, es

; 	push bx cx ax di
; 	mov cx, cs
; 	mov es, cx
; 	call str_len                                  ; TRASHES: al, di, result in cx
; 	pop di
; 	xchg si, di

; 	pop ax
; 	mov dh, al
; 	sub dh, cl
; 	sar dh, 1
; 	add bl, dh
; 	pop cx
; 	call print_text                               ; TRASHES: ax, bx, dx, si, es

; 	pop bx
; 	add bl, MSG_BOX_TEXT_HORIZONTAL_OFFSET
; 	add bh, MSG_BOX_TEXT_VERTICAL_OFFSET
; 	mov si, di
; 	call print_text                               ; TRASHES: ax, bx, dx, si, es

; 	ret

; msg_box_anim endp


; ;-------------------------------------------
; ; display frame with animation
; ;
; ; bl - column offset
; ; bh - row offset
; ; al - width
; ; ah - height
; ; ch - color attribute
; ;-------------------------------------------
; ; TRASHES: dx, es
; ;-------------------------------------------
; show_frame proc
; 	mov dl, 02h
; 	mov dh, 02h

; show_frame_horizontal_expand_loop:
; 	push ax bx cx dx
; 	call draw_frame                            ; TRASHES: ax, bx, cx, dx, es
; 	pop  dx cx bx ax

; 	push ax cx dx
; 	call delay                                 ; TRASHES: ah, cx, dx
; 	pop  dx cx ax

; 	inc dl
; 	cmp dl, al
; 	jb show_frame_horizontal_expand_loop
	
; show_frame_vertical_expand_loop:
; 	push ax bx cx dx
; 	call draw_frame                            ; TRASHES: ax, bx, cx, dx, es
; 	pop  dx cx bx ax

; 	push ax cx dx
; 	call delay                                 ; TRASHES: ah, cx, dx
; 	pop  dx cx ax

; 	inc dh
; 	cmp dh, ah
; 	jb show_frame_vertical_expand_loop	
	
; 	ret
	
; show_frame endp


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

	push bx cx dx
	call draw_line           ; TRASHES: ax, bl, cl, dl, es
	pop  dx cx bx

	push bx cx dx
	call draw_line_shadow    ; TRASHES: ax, bx, cx, dl, es
	pop  dx cx bx
	
	mov cl, 'º'
	mov al, ' '
	mov ah, 'º'

	dec dh		             ; to correct frame height

draw_frame_loop:
	inc bh

	push ax bx cx dx
	call draw_line           ; TRASHES: ax, bl, cl, dl, es
	pop  dx cx bx ax

	push ax bx cx dx
	call draw_line_shadow    ; TRASHES: ax, bx, cx, dl, es
	pop  dx cx bx ax
	
	dec dh
	jnz draw_frame_loop
	
	mov cl, 'È'
	mov al, 'Í'
	mov ah, '¼'

	push bx cx dx
	call draw_line            ; TRASHES: ax, bl, cl, dl, es
	pop  dx cx bx

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
	push ax bx dx
	call put_char      ; TRASHES: ax, bx, dx, es
	pop  dx bx ax
	
	mov cl, al
	dec dl		       ; to correct line width

draw_line_loop:
	inc bl

	push ax bx dx
	call put_char      ; TRASHES: ax, bx, dx, es
	pop  dx bx ax
	
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