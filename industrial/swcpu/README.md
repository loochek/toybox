# SWCPU
Here is an assembly language and a toolchain for it: an assembler, a disassembler and, in fact, the SWCPU itself, which executes assembled programs.
## Assembly language
The grammar is as follows:
* One instruction - one line
* The line can be empty, contain an instruction, a comment after an instruction, contain only a comment
> ; Line with comment only  
> pop rax  
> ; ^^^ Line with instruction only  
> pop rax ; Line with both instruction and comment  
>  
> ; ^^^ Empty line  
* The instruction can take one argument.
> nop     ; Without argument  
> pop rax ; With argument  
* The argument can be "lvalue" or rvalue". "Rvalue" is a calculated constant. "Lvalue" is some kind of memory cell, like a register or a RAM cell
> push rax ; Push value from register rax - great!  
> pop  15  ; Pop value from 15 - what?  
* Argument is a combination of memory, register, immediate modifiers
> rax      ; both lvalue and rvalue  
> rax+5    ; rvalue  
> 5        ; rvalue  
> [rax+-5] ; both lvalue and rvalue  
> [rax]    ; both lvalue and rvalue  
> [10]     ; both lvalue and rvalue  
* The argument cannot contain spaces (a compromise to simplify assembler)
* There are labels: assembler-time calculated values. The most common use case is jumps
## Instruction set:
* push rvalue - push value on the stack
* pop  lvalue - pop value from the stack to lvalue
* nop         - do nothing
* hlt         - stop execution
* in          - request a number from the user and push it on the stack
* out         - pop value from the stack and print it
* jmp offset  - set the instruction pointer to offset
* je  offset
* jne offset
* jl  offset
* jle offset
* jg  offset
* jge offset  - conditional jumps - takes two arguments from the stack and compares it, jump if condition is true, else behave like a nop
* call offset - jump to subroutine - saves the return address on the stack
* ret         - return from subroutine - takes the return address on the stack
* add
* sub
* mul
* div         - binary operators - takes two arguments from the stack and pushes the result
* sqrt        - unary operator - takes one arguments from the stack and pushes the result
* fbupd       - show VRAM contents to the screen (if framebuffer enabled)
## CPU capabilities (configurable)
* Up to 26 registers - rax...rzx
* RAM
* CPU deals with floating-point numbers - if integer required (as memory/code offset/etc) fractional part discards
* VRAM - Higher 128x128 cells of RAM can be displayed in the Linux framebuffer. Color is encoded as 3-digits base 256 number - bgr
## Binary encoding
* The opcode consists of two parts - the 5 most significant bits of the base opcode and the 3 least significant bits of the argument mask (see cpu_def.h)
* Byte order problem (yet) not resolved - do not port assembled programs to architectures with different byte ordering
* Immediate arguments is located after opcode (see cpu_def.h)
