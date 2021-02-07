.model tiny
.code

org 0100h

start:
	mov si, offset msg ; current symbol address
	mov ax, 0B800h + 000Ah
	mov es, ax
	mov di, 0000h
	mov cl, 01h

print_loop:
	; check for terminate symbol
	cmp [si], byte ptr 24h
	je print_loop_end

	; Put symbol
	mov ax, [si]
        mov es:[di], ax
	inc di
	mov es:[di], cl
    	inc di
        inc si
	inc cl
        
	cmp cl, 08h
	jne color_reset_skip
	mov cl, 01h

color_reset_skip:	
	jmp print_loop

print_loop_end:	
	ret

msg:
	db "It is a long message, so, I hope, it doesn't fit on one line of default DOS video mode", '$'

end start
