;-------------------------------------------
; es:di - address of $-terminated string
; result in cx
;-------------------------------------------
; TRASHES: al, di
;-------------------------------------------
str_len proc
	mov al, '$'
	xor cx, cx
	dec cx
	cld
	repne scasb
	neg cx
	sub cx, 2

	ret

str_len endp


;-------------------------------------------
; animation delay
; sleep for 25ms
;-------------------------------------------
; TRASHES: ah, cx, dx
;-------------------------------------------
delay proc
	mov dx, 61A8h
	xor cx, cx
	mov ah, 86h
	int 15h

	ret

delay endp