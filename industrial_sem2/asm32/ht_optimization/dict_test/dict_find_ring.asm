section .text

global _Z14dict_find_ringP6dict_tPKcPP12chain_ring_t

extern _Z14dict_calc_hashPKc
extern _Z9list_dataI12chain_ring_tE9lstatus_tP6list_tIT_E11list_iter_tPPS3_
extern _Z9list_nextI12chain_ring_tE9lstatus_tP6list_tIT_EP11list_iter_t
extern _Z10list_beginI12chain_ring_tE9lstatus_tP6list_tIT_EP11list_iter_t
extern _Z8list_endI12chain_ring_tE9lstatus_tP6list_tIT_EP11list_iter_t
extern _Z13list_iter_cmp11list_iter_tS_

extern strcmp

%define dict_find_ring _Z14dict_find_ringP6dict_tPKcPP12chain_ring_t
%define dict_calc_hash _Z14dict_calc_hashPKc
%define list_data      _Z9list_dataI12chain_ring_tE9lstatus_tP6list_tIT_E11list_iter_tPPS3_
%define list_next      _Z9list_nextI12chain_ring_tE9lstatus_tP6list_tIT_EP11list_iter_t
%define list_begin     _Z10list_beginI12chain_ring_tE9lstatus_tP6list_tIT_EP11list_iter_t
%define list_end       _Z8list_endI12chain_ring_tE9lstatus_tP6list_tIT_EP11list_iter_t
%define list_iter_cmp  _Z13list_iter_cmp11list_iter_tS_

; lstatus_t dict_find_ring(dict_t *dict, const char *key, chain_ring_t **ring_out)
;                                   rdi               rsi                 rdx
dict_find_ring:
    push r12
    push r13
    push r14
    push r15

    sub rsp, 24

    mov r12, rdi
    mov r13, rsi
    mov r14, rdx

; int hash = dict_calc_hash(key) % dict->buckets_count;
    mov rdi, r13
    call dict_calc_hash
    mov ecx, [r12 + 16]  ; dict->buckets_count
    xor edx, edx
    idiv ecx
    mov r15d, edx
; hash in r15d

; list_t<chain_ring_t> *bucket = &dict->buckets[hash];
; bucket = [dict + 4] + hash * 48
; rax = bucket address
    mov rax, [r12 + 8]  ; &dict->buckets
    mov ecx, r15d
    shl rcx, 4 ; * 16
    lea rcx, [rcx + rcx * 2] ; * 3
    lea rax, [rax + rcx]
    mov r15, rax
; bucket address in r15

; list_iter_t iter = NULLITER, end_iter = NULLITER;
    mov [rsp], dword 0
    mov [rsp + 4], dword 0

; list_begin(bucket, &iter);
; list_end  (bucket, &end_iter);
    mov rdi, r15
    lea rsi, [rsp]
    call list_begin
    test eax, eax
    jnz end

    mov rdi, r15
    lea rsi, [rsp + 4]
    call list_end
    test eax, eax
    jnz end

; while (!list_iter_cmp(iter, end_iter))
    mov esi, [rsp]
    mov edi, [rsp + 4]
    call list_iter_cmp
    test eax, eax
    jnz while_end

while:
; list_data(bucket, iter, &curr_ring))
    mov rdi, r15
    mov rsi, [rsp]
    lea rdx, [rsp + 8]
    call list_data
    test eax, eax
    jnz end
; chain_ring_t* in [rsp+8] right now

; strcmp(curr_ring->key, key)
    mov rdi, [rsp + 8]
    mov rdi, [rdi]
    mov rsi, r13
    call strcmp
    test rax, rax
    jnz if_skip

; *ring_out = curr_ring
    mov rdi, [rsp + 8]
    mov [r14], rdi
    xor eax, eax
    jmp end

if_skip:
    mov rdi, r15
    mov rsi, rsp
    call list_next
    test eax, eax
    jnz end

    mov esi, [rsp]
    mov edi, [rsp + 4]
    call list_iter_cmp
    test eax, eax
    jz while

while_end:
    mov eax, 8

end:
    add rsp, 24
    pop r15
    pop r14
    pop r13
    pop r12
    ret