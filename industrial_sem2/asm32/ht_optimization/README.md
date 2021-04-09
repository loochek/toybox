 
(Template - WIP)

We have a English-to-Russian dictionary based on a chaining hash table. Words definitions are loaded from file, you can write an English word and get a simple dictionary topic.
So, there are hash table with pointers to strings as keys and values, contents of the dictionary are loaded as a big buffer, then simple parser constructs in-place C strings and inserts pointers to the hash table. In a nutshell, a pretty efficient structure.

But the main challenge is to optimize the hash table using assembly. First, we need to profile our application in order to spot functions which optimization will really speed up the application. I'm using Callgrind with Kcachegrind as a frontend. Second, we must profile and test the program in real-world use cases. In our case, it's important to speed up the search in the dictionary, since data loading and insertion into the dictionary occurs only once at the start of the program, while the search takes the rest of the program's operation time. 

To make a test that respects the comments described, I took the dictionary client and replaced the user interaction loop with repeated dictionary searches. With this approach, things like file I/O and memory allocations have little effect on the profiling results, allowing us to see real bottlenecks.

At this point, we can start profiling. But which keys we should take for such repeated queries? As profiling results depends on word's length (see appendix), I took all keys from the dictionary and use random key for each query. Profiling with two different rands (built-in and xor shift) shows that hashes calculation takes most of the time. The second place is taken by the dict_find_ring function.

According to the profiling results, we should revise hash calculation algorithm at first. Now I'm using polynominal hash, but it isn't a good choice. I've found an elegant algorithm based on bit shifts. As we know, multiplication and, especially, division are much more heavier than bit shifts. So, let's try to replace the hash function and re-measure time. Even slower! There are mismatch between our expectations and the result. Let's go deeper and take a look to the compilier output.

Polynominal hash:
```
00000000000006b0 <_Z14dict_calc_hashPKc>:
 6b0:   55                      push   rbp
 6b1:   48 89 e5                mov    rbp,rsp
 6b4:   48 89 7d f8             mov    QWORD PTR [rbp-0x8],rdi
 6b8:   48 c7 45 f0 00 00 00    mov    QWORD PTR [rbp-0x10],0x0
 6bf:   00 
 6c0:   48 8b 45 f8             mov    rax,QWORD PTR [rbp-0x8]
 6c4:   48 89 45 e8             mov    QWORD PTR [rbp-0x18],rax
 6c8:   48 8b 45 e8             mov    rax,QWORD PTR [rbp-0x18]
 6cc:   0f be 08                movsx  ecx,BYTE PTR [rax]
 6cf:   83 f9 00                cmp    ecx,0x0
 6d2:   0f 84 35 00 00 00       je     70d <_Z14dict_calc_hashPKc+0x5d>
 6d8:   48 69 45 f0 1f 00 00    imul   rax,QWORD PTR [rbp-0x10],0x1f
 6df:   00 
 6e0:   48 8b 4d e8             mov    rcx,QWORD PTR [rbp-0x18]
 6e4:   48 0f be 09             movsx  rcx,BYTE PTR [rcx]
 6e8:   48 01 c8                add    rax,rcx
 6eb:   31 d2                   xor    edx,edx
 6ed:   b9 07 ca 9a 3b          mov    ecx,0x3b9aca07
 6f2:   48 f7 f1                div    rcx
 6f5:   48 89 55 f0             mov    QWORD PTR [rbp-0x10],rdx
 6f9:   48 8b 4d e8             mov    rcx,QWORD PTR [rbp-0x18]
 6fd:   48 81 c1 01 00 00 00    add    rcx,0x1
 704:   48 89 4d e8             mov    QWORD PTR [rbp-0x18],rcx
 708:   e9 bb ff ff ff          jmp    6c8 <_Z14dict_calc_hashPKc+0x18>
 70d:   48 8b 45 f0             mov    rax,QWORD PTR [rbp-0x10]
 711:   5d                      pop    rbp
 712:   c3                      ret
 ```

Bit shift hash:
```
00000000000006b0 <_Z14dict_calc_hashPKc>:
 6b0:   55                      push   rbp
 6b1:   48 89 e5                mov    rbp,rsp
 6b4:   48 89 7d f8             mov    QWORD PTR [rbp-0x8],rdi
 6b8:   c7 45 f4 00 00 00 00    mov    DWORD PTR [rbp-0xc],0x0
 6bf:   48 8b 45 f8             mov    rax,QWORD PTR [rbp-0x8]
 6c3:   0f be 08                movsx  ecx,BYTE PTR [rax]
 6c6:   83 f9 00                cmp    ecx,0x0
 6c9:   0f 84 38 00 00 00       je     707 <_Z14dict_calc_hashPKc+0x57>
 6cf:   48 8b 45 f8             mov    rax,QWORD PTR [rbp-0x8]
 6d3:   0f be 08                movsx  ecx,BYTE PTR [rax]
 6d6:   03 4d f4                add    ecx,DWORD PTR [rbp-0xc]
 6d9:   89 4d f4                mov    DWORD PTR [rbp-0xc],ecx
 6dc:   8b 4d f4                mov    ecx,DWORD PTR [rbp-0xc]
 6df:   c1 e1 0a                shl    ecx,0xa
 6e2:   03 4d f4                add    ecx,DWORD PTR [rbp-0xc]
 6e5:   89 4d f4                mov    DWORD PTR [rbp-0xc],ecx
 6e8:   8b 4d f4                mov    ecx,DWORD PTR [rbp-0xc]
 6eb:   c1 e9 06                shr    ecx,0x6
 6ee:   33 4d f4                xor    ecx,DWORD PTR [rbp-0xc]
 6f1:   89 4d f4                mov    DWORD PTR [rbp-0xc],ecx
 6f4:   48 8b 45 f8             mov    rax,QWORD PTR [rbp-0x8]
 6f8:   48 05 01 00 00 00       add    rax,0x1
 6fe:   48 89 45 f8             mov    QWORD PTR [rbp-0x8],rax
 702:   e9 b8 ff ff ff          jmp    6bf <_Z14dict_calc_hashPKc+0xf>
 707:   8b 45 f4                mov    eax,DWORD PTR [rbp-0xc]
 70a:   c1 e0 03                shl    eax,0x3
 70d:   03 45 f4                add    eax,DWORD PTR [rbp-0xc]
 710:   89 45 f4                mov    DWORD PTR [rbp-0xc],eax
 713:   8b 45 f4                mov    eax,DWORD PTR [rbp-0xc]
 716:   c1 e8 0b                shr    eax,0xb
 719:   33 45 f4                xor    eax,DWORD PTR [rbp-0xc]
 71c:   89 45 f4                mov    DWORD PTR [rbp-0xc],eax
 71f:   8b 45 f4                mov    eax,DWORD PTR [rbp-0xc]
 722:   c1 e0 0f                shl    eax,0xf
 725:   03 45 f4                add    eax,DWORD PTR [rbp-0xc]
 728:   89 45 f4                mov    DWORD PTR [rbp-0xc],eax
 72b:   8b 45 f4                mov    eax,DWORD PTR [rbp-0xc]
 72e:   5d                      pop    rbp
 72f:   c3                      ret
```

As we see, compilier access memory very intensely - it seems to be a main problem. The bit shift version has more intensive memory usage than the polynomial version, so our expectations about divisions and bit shifts turned out to be insignificant against the frequent memory accesses. Let's write the bit shift version manually in assembly and compare.

There are result:
```
_Z14dict_calc_hashPKc:
    xor eax, eax

    mov cl, [rdi]
    test cl, cl
    jz end

loop:
    movzx esi, cl
    add eax, esi
    mov esi, eax
    shl esi, 10
    add eax, esi
    mov esi, eax
    shr esi, 6
    xor eax, esi
    
    inc rdi
    mov cl, [rdi]
    test cl, cl
    jnz loop

end:
    mov esi, eax
    shl esi, 3
    add eax, esi
    mov esi, eax
    shr esi, 11
    xor eax, esi
    mov esi, eax
    shl esi, 15
    add eax, esi

    ret
```

Our version is much more lightweight - there are only one memory access per symbol. Let's compare. Comparing initial polynominal hashing and optimized bit shift hashing, we have achieved ~20% boost! Let's see profilier output now. Another heavily-used function is dict_find_ring
