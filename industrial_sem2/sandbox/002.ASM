.model tiny
.code

org 0100h

start:
	mov ah, 09h
	mov dx, offset str_hello
	int 21h	
	
	mov ah, 09
	mov dx, offset str_cs
	int 21h
	
	mov ax, cs
	call print_ax

	call print_enter
	
    	mov ah, 09
	mov dx, offset str_ds
	int 21h
	
	mov ax, ds
	call print_ax

	call print_enter
	
	mov ah, 09
	mov dx, offset str_es
	int 21h
	
	mov ax, es
	call print_ax

	call print_enter
	
	mov ah, 09
	mov dx, offset str_ss
	int 21h
	
	mov ax, ss
	call print_ax

	call print_enter

	ret

print_ax:
	mov di, offset str_pos
	call itoa
	
	mov ah, 09h
	mov dx, offset str_pos
	int 21h
	
	ret

print_enter:
	mov ah, 09h
	mov dx, offset enter_str
	int 21h

	ret
	
include itoa.asm

str_pos:
	db 10 dup(0)

enter_str:
	db 0Dh, 0Ah, '$'

str_hello:
	db "Registers state:", 0Dh, 0Ah, '$'

str_cs:
	db "CS: ", '$'

str_ds:
	db "DS: ", '$'

str_es:
	db "ES: ", '$'

str_ss:
	db "SS: ", '$'

end start	
