.model tiny
.code

org 0100h

start:
	mov ax, 0ABADh
	mov bx, 0BABEh
	mov cx, 0DEADh
	mov dx, 0BEEFh

inf_loop:
	inc cx
	jmp inf_loop

	ret

end start