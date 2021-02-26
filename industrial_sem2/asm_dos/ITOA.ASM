; converts unsigned 16-bit integer from ax to $-terminated string at ds:[di]

itoa:
	push '@' ; our custom terminator to mark the end of loop
	push '$'

itoa_digit_loop:	
	cmp ax, 0000h
	je itoa_digit_loop_end

	mov dx, 0000h
	mov bx, 000Ah
	div bx
	add dx, '0'
	push dx
	
	jmp itoa_digit_loop

itoa_digit_loop_end:
	pop ax

itoa_str_loop:
	cmp ax, '@'
	je itoa_str_loop_end

	mov [di], ax
	inc di
	pop ax
	jmp itoa_str_loop	
	
itoa_str_loop_end:
	ret