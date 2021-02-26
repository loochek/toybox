.model tiny
.code

org 0100h

HOTKEY     equ 36h ; right shift
ACTIVE_KEY equ 2Ah ; left shift
CRASH_KEY  equ 0Eh ; backspace
WATCH1_KEY equ 02h ; 1
WATCH2_KEY equ 03h ; 2
WATCH3_KEY equ 04h ; 3
WATCH4_KEY equ 05h ; 4

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

    mov watch_cfg_ptr, offset watch1_cfg    ; initialize variable with default value

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

; check if hotkey pressed or released
    cmp al, HOTKEY
    je keyb_int_handler_hotkey_pressed

    cmp al, HOTKEY + 80h    ; released scancode
    je keyb_int_handler_hotkey_released

; check hotkey status, exit if not pressed
    cmp cs:hotkey_pressed, 0FFh
    jne keyb_int_handler_exit

; if hotkey pressed, check function keys
    cmp al, CRASH_KEY
    je keyb_int_handler_crash

    cmp al, WATCH1_KEY
    je keyb_int_handler_watch1

    cmp al, WATCH2_KEY
    je keyb_int_handler_watch2

    cmp al, WATCH3_KEY
    je keyb_int_handler_watch3

    cmp al, WATCH4_KEY
    je keyb_int_handler_watch4

    cmp al, ACTIVE_KEY
    je keyb_int_handler_active

; exit if no function keys pressed
keyb_int_handler_exit:
    pop ax
    push cs:old_handler_segment
    push cs:old_handler_offset
    retf


; keys handlers
;-------------------------------------------
keyb_int_handler_crash:
    call crash
    ; never returns

;-------------------------------------------
keyb_int_handler_hotkey_pressed:
    mov cs:hotkey_pressed, 0FFh
    jmp keyb_int_handler_exit

;-------------------------------------------
keyb_int_handler_hotkey_released:
    mov cs:hotkey_pressed, 00h
    jmp keyb_int_handler_exit

;-------------------------------------------
keyb_int_handler_watch1:
    mov cs:watch_cfg_ptr, offset watch1_cfg
    jmp keyb_int_handler_show_regmon

;-------------------------------------------
keyb_int_handler_watch2:
    mov cs:watch_cfg_ptr, offset watch2_cfg
    jmp keyb_int_handler_show_regmon
    
;-------------------------------------------
keyb_int_handler_watch3:
    mov cs:watch_cfg_ptr, offset watch3_cfg
    jmp keyb_int_handler_show_regmon
    
;-------------------------------------------
keyb_int_handler_watch4:
    mov cs:watch_cfg_ptr, offset watch4_cfg
    jmp keyb_int_handler_show_regmon

;-------------------------------------------
keyb_int_handler_active:
    ; just jump - update last active watch
    jmp keyb_int_handler_show_regmon
    
; common code for previous handlers
; shows Registers Monitor depending on watch_cfg
;-------------------------------------------
keyb_int_handler_show_regmon:
    pop ax
    push ax

    push ax bx cx dx si di ds es
    mov si, cs
    mov ds, si

    mov di, watch_cfg_ptr
    mov si, [di]
    add di, 2

    call regmon_show
    pop es ds di si dx cx bx ax

    jmp keyb_int_handler_exit

keyb_int_handler endp


;-------------------------------------------
; infinitely transfer RAM to VRAM
;-------------------------------------------
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


;-------------------------------------------
; variables
;-------------------------------------------
old_handler_segment   dw 0
old_handler_offset    dw 0
hotkey_pressed        db 0
watch_cfg_ptr         dw 0


;-------------------------------------------
; watches windows parameters
;-------------------------------------------
watch1_cfg:
    db 1
    db 1
    db "Watch 1", '$'

watch2_cfg:
    db SCREEN_WIDTH - REGMON_WINDOW_WIDTH - 1
    db 1
    db "Watch 2", '$'

watch3_cfg:
    db 1
    db SCREEN_HEIGHT - REGMON_WINDOW_HEIGHT - 3
    db "Watch 3", '$'

watch4_cfg:
    db SCREEN_WIDTH - REGMON_WINDOW_WIDTH - 1
    db SCREEN_HEIGHT - REGMON_WINDOW_HEIGHT - 3
    db "Watch 4", '$'


include REGMON.ASM

REZIDENT_MEM_SIZE:          ; want to keep all program as resident
end start