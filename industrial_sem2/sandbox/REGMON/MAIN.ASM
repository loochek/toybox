.model tiny
.code

org 0100h

HOTKEY_SCANCODE      equ 36h ; right shift
WINDOW_SHOW_SCANCODE equ 2ah ; left shift
CRASH_SCANCODE       equ 29h ; ~

ARROW_UP_SCANCODE    equ 48h
ARROW_DOWN_SCANCODE  equ 50h
ARROW_LEFT_SCANCODE  equ 4bh
ARROW_RIGHT_SCANCODE equ 4dh

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
    mov dx, offset REZIDENT_MEM_SIZE
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
    je keyb_int_handler_hotkey_pressed

    cmp al, HOTKEY_SCANCODE + 80h
    je keyb_int_handler_hotkey_released

    cmp cs:hotkey_pressed, 0FFh
    jne keyb_int_handler_exit

    cmp al, CRASH_SCANCODE
    je keyb_int_handler_crash

    cmp al, WINDOW_SHOW_SCANCODE
    jne keyb_int_handler_exit

    pop ax
    push ax                                 ; restore trashed al

    push bx cx dx si di ds es
    push cx
    mov cx, cs
    mov ds, cx
    pop cx

    call regmon_show

    pop es ds di si dx cx bx
    jmp keyb_int_handler_exit

keyb_int_handler_crash:
    call crash

keyb_int_handler_hotkey_pressed:
    mov cs:hotkey_pressed, 0FFh
    jmp keyb_int_handler_exit

keyb_int_handler_hotkey_released:
    mov cs:hotkey_pressed, 00h

keyb_int_handler_exit:
    pop ax
    push cs:old_handler_segment
    push cs:old_handler_offset
    retf

keyb_int_handler endp

crash proc
    mov cx, 0B800h
    mov es, cx
    mov cx, 0
    mov ds, cx
    xor si, si
    xor di, di

crash_loop:
    mov cx, 80 * 25 * 2
    rep movsb
    xor di, di
    jmp crash_loop

crash endp

old_handler_segment dw 0
old_handler_offset  dw 0
hotkey_pressed      db 0

window_collumn_offset db 1
window_row_offset     db 1

include REGMON.ASM

REZIDENT_MEM_SIZE:          ; want to keep all program as resident

end start