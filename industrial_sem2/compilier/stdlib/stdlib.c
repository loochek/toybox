#define __STR(x) #x
#define STR(s) __STR(s)

#define BUFFER_LENGTH 32

long long print(long long x);

long long input()
{
    char buffer[32];

    int bytes_read = 0;
    asm volatile
    (
        ".intel_syntax noprefix\n"
        "xor rax, rax\n"
        "mov rdi, 1\n"
        "mov rdx, " STR(BUFFER_LENGTH - 1) "\n"
        "syscall\n"
        : "=a" (bytes_read)
        : "S"(buffer)
        : "rdi", "rdx", "memory"
    );

    buffer[bytes_read] = '\0';

    print(bytes_read);

    long long num = 0;
    char sign = 1;
    
    char *ptr = buffer;
    while (!('0' <= *ptr && *ptr <= '9' || *ptr == '+' || *ptr == '-'))
        ptr++;
    
    if (*ptr == '-')
    {
        sign = -1;
        ptr++;
    }
    else if (*ptr == '+')
        ptr++;
    
    while ('0' <= *ptr && *ptr <= '9')
    {
        num *= 10;
        num += *ptr - '0';
        ptr++;
    }
    
    return num * sign;
}

long long print(long long x)
{
    char buffer[32] = {0};
    
    // last pos
    char *ptr = buffer + sizeof(buffer) - 1;
    
    *ptr = '\0';
    *(ptr - 1) = '\n';
    ptr -= 2;
    
    int sign = 1;
    
    if (x < 0)
    {
        sign = -1;
        x = -x;
    }
    
    while (x > 0)
    {
        *ptr = '0' + x % 10;
        ptr--;
        x /= 10;
    }
    
    if (sign < 0)
        *ptr = '-';
    else
        ptr++;
    
    int len = buffer + sizeof(buffer) - ptr;
    
    asm volatile
    (
        ".intel_syntax noprefix\n"
        "mov rax, 1\n"
        "mov rdi, 0\n"
        "syscall\n"
        :
        : "S"(ptr), "d"(len)
        : "rax", "rdi", "memory"
    );
    
    return 0;
}
