.model tiny
.code

org 100h

fake_start:

org 012Eh
	db "MEOW"	; payload beginning - you need to cut zero bytes before MEOW in result file

org 0140h
	nop			; for safety
	nop
	dw 48EBh		; original bytes - must keep them!
	dd 0000B901h
	dw 09B4h
	dw 0B8BAh
	db 01h
	dw 021CDh
	dw 0C985h
	dw 0F574h
	dw 43BBh
	db 01h
	dd 012E07C7h
	dw 00B8h
	db 4Ch
	dw 21CDh

org 015Fh

; write your exploit here!
start proc
	mov ax, 176h	; just skip password check
	jmp ax

start endp

org 0176h	; there should be a warning if your exploit is too big, but TASM is stupid

end fake_start