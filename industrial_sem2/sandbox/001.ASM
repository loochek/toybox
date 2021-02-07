.model tiny
.code

org 0100h

start:
	mov si, offset msg ; current symbol address
	mov ax, 0B800h
	mov es, ax
	mov di, 0	

print_loop:
	; check for terminate symbol
	cmp [si], byte ptr 24h
	je print_loop_end

	; Put symbol
	mov ax, [si]
        mov es:[di], ax
	inc di
	mov es:[di], byte ptr 7
    	inc di
	        
        inc si
        jmp print_loop

print_loop_end:	
	ret

msg:
	db "test test test test test", '$'

end start
