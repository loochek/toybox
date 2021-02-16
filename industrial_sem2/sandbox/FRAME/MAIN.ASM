.model tiny
.code

org 0100h

HOTKEY_SCANCODE equ 36h ; right shift

;-------------------------------------------
; main
; sets hook to the keyboard interrupt
;-------------------------------------------
start proc
    xor bx, bx
    mov es, bx
    mov bx, 4 * 9h       ; INT 9h address location

    cli
    mov si, es:[bx + 2]
    mov di, es:[bx]
    mov old_handler_segment, si
    mov old_handler_offset,  di

    mov si, cs
    mov es:[bx + 2], si
    mov es:[bx]    , offset keyb_int_handler
    sti

    mov ax, 3100h   ; TSR
    mov dx, word ptr REZIDENT_MEM_SIZE
    shr dx, 4
    inc dx
    int 21h

    ; restore old handler in case of fail

    cli
    mov si, old_handler_segment
    mov di, old_handler_offset
    mov es:[bx + 2], si
    mov es:[bx]    , di
    sti

    ret

start endp


;-------------------------------------------
; keyboard interrupt handler
;-------------------------------------------
keyb_int_handler proc
    push ax
    in al, 60h
    cmp al, HOTKEY_SCANCODE
    pop ax
    jne keyb_int_handler_exit

    push ax bx cx dx si di ds es
    mov cx, cs
    mov ds, cx

    call regmon_show

    pop es ds di si dx cx bx ax

keyb_int_handler_exit:
    push cs:old_handler_segment
    push cs:old_handler_offset
    retf

keyb_int_handler endp

old_handler_segment dw 0
old_handler_offset  dw 0

include REGMON.ASM

REZIDENT_MEM_SIZE:          ; want to keep all program as resident

end start