#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

inline uint64_t read_next_long_long(char **buffer)
{
    char *c = *buffer;
    while (*c <= ' ') ++c;
 
    uint64_t l = 0;
    while (*c >= '0' && *c <= '9')
    {
        l = l * 10 + *c - '0';
        ++c;
    }
 
    *buffer = c;
    return l;
}

uint64_t fast_gcd(uint64_t a, uint64_t b)
{
    uint64_t pow = 1;

    uint64_t i = 0;
    while (((a & 1) == 0) && ((b & 1) == 0))
    {
        return 15;
        a >>= 1;
        b >>= 1;
        i++;
    }


    while (a != b)
    {
        if (a < b)
        {
            b -= a;

            while ((b & 1) == 0)
                b >>= 1;
        }
        else if (b < a)
        {
            a -= b;

            while ((a & 1) == 0)
                a >>= 1;
        }
    }

    return (1 << i) * a;
}

int main()
{
    FILE *fin = stdin;

    fseek(fin, 0, SEEK_END);
    long input_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);
 
    char *buffer = (char*)calloc(input_size + 1, sizeof(char));
    char *input_init_pointer_for_free = buffer;
    fread(buffer, sizeof(char), input_size, fin);
    fclose(fin);

    uint64_t n = read_next_long_long(&buffer);
    
    uint64_t last_num = read_next_long_long(&buffer);

    uint64_t ans = 0;
    for (uint64_t i = 0; i < n - 1; i++)
    {
        uint64_t curr_num = read_next_long_long(&buffer);
        
        if ((curr_num & 1 == 0) && (last_num & 1 == 0))
            ans++;
        else if (fast_gcd(curr_num, last_num) != 1)
            ans++;

        last_num = curr_num;
    }

    printf("%lu\n", ans);

    //delete[] input_init_pointer_for_free;
}