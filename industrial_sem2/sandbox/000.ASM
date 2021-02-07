.model tiny
.code

org 0100h

start:
	; init counters
	mov si, 0000h ; string pos
	mov di, 0001h ; color

print_loop:
	; check for terminate symbol
	cmp [si + offset msg], byte ptr 24h
	je print_loop_end

	; Put symbol
	mov al, [si + offset msg]
	mov ah, 0009h
	mov cx, 0001h
	mov bx, di
	int 10h
	
    	inc di
	cmp di, 0008h
	jne color_reset_skip
	mov di, 0001h

color_reset_skip:
	; Move cursor
	mov ah, 03h
	mov bh, 00h
	int 10h
	mov ah, 02h
	mov bh, 00h
	inc dl
	int 10h
        
        inc si
        jmp print_loop

print_loop_end:	
	ret

msg:
	db "Hello world!", "$"

end start
