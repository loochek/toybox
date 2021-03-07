.model tiny
.code

org 0100h

start proc

	mov ah, 09h
	mov dx, offset hello_text
	int 21h
	
	mov di, offset passwd_buf
	cld
	mov ah, 08h		; 01h for debug, 08h for release

passwd_loop:
	int 21h

	cmp al, 0Dh ; carriage return aka Enter
	je passwd_loop_end

	stosb

	jmp passwd_loop

passwd_loop_end:
	mov si, offset passwd_buf
	mov di, offset correct_passwd
	mov cx, correct_passwd_len

passwd_check_loop:
	lodsb
	mov ah, decrypt_key
	xor al, ah
	scasb
	jne passwd_incorrect

	loop passwd_check_loop

	jmp passwd_correct

passwd_incorrect:
	mov ah, 09h
	mov dx, offset incorrect_passwd_react
	int 21h
	ret

passwd_correct:
	mov ah, 09h
	mov dx, offset correct_passwd_react
	int 21h
	ret

endp

hello_text             db "Enter a password and press Enter:", 0Ah, 0Dh, "$"

correct_passwd_react   db "Access granted", 0Ah, 0Dh
                       db "Welcome to FBI Secret Network!", 0Ah, 0Dh
					   db "Segmentation fault$"

incorrect_passwd_react db "Incorrect password, access denied$"


passwd_buf     db correct_passwd_len dup(0)

correct_passwd dq 0C4C3C7CECBC3C8CCh

correct_passwd_len equ $ - correct_passwd

decrypt_key    db 0AAh

end start