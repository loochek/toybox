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
	call check_password
	test al, al
	jz passwd_incorrect
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

db 10 dup(0)  ; fake buffer and offset

;------------------------------------------
; checks password given in si to be correct
; puts 01 or 00 to al
;------------------------------------------
; TRASHES: ah, cx, si, di
;------------------------------------------
check_password:
	mov di, offset correct_passwd
	mov cx, correct_passwd_len

check_password_loop:
	lodsb
	mov ah, decrypt_key
	xor al, ah
	scasb
	jne check_password_incorrect

	loop check_password_loop

	; password is correct
	xor al, al
	inc al
	ret

check_password_incorrect:
	xor al, al
	ret

end start