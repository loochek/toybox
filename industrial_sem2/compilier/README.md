# Tiny Compilier
This is a small programming language and compiler which able to create AMD64 ELF binaries.

This project is a continuation of the [compiler](../../industrial_sem1/compilier) from the first semester, revised, heavily rewritten and with a new backend (previous compiler creates assembly code for a self-written virtual machine). Comparing between the native execution and previous virtual machine execution, there is a performance boost of about 200 times ([test](speedup_tests/fib.tc) - looped Fibonacci number counting; virtual machine was compiled with O3 and disabled debug features; hardware is Ryzen 3500U).

## How to build
Just `make`. Requires Clang (GCC is not supported now) and Python 3 with pyelftools package.

## If you are interested
- [Language syntax and semantics](LANGUAGE.md)
- [Description of compiling process](COMPILER.md)
- [Sample programs](examples/)
