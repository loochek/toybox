#include <stdio.h>
#include <stdlib.h>

size_t matrix[2000][64];

int main()
{

    size_t n = 0;
    scanf("%zu", &n);

    size_t n_div = (n >> 5) + 1;

    for (size_t i = 0; i < n; i++)
    {
        size_t flush_cnt = 0;
        for (size_t j = 0; j < n; j++)
        {
            size_t bit = getchar();
            if (bit == '\n')
                bit = getchar() - '0';
            else
                bit -= '0';

            if (bit)
                matrix[i][j >> 5] |= (1 << flush_cnt);

            flush_cnt++;
            flush_cnt %= 32;
        }
    }

    // for (size_t i = 0; i < n; i++)
    // {
    //     for (size_t j = 0; j < n; j++)
    //     {
    //         size_t bit = getchar();
    //         if (bit == '\n')
    //             bit = getchar() - '0';
    //         else
    //             bit -= '0';

    //         matrix[i][j] = bit;
    //     }
    // }

    for (size_t k = 0; k < n; k++)
    {
        for (size_t i = 0; i < n; i++)
        {
            if ((matrix[i][k >> 5] >> (k % 32)) & 1)
            {
                for (size_t j = 0; j < n_div; j++)
                {
                    matrix[i][j] |= matrix[k][j];
                }
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        size_t flush_cnt = 0;
        for (int j = 0; j < n; j++)
        {
            printf("%zu", (matrix[i][j >> 5] >> flush_cnt) & 1);

            flush_cnt++;
            flush_cnt %= 32;
        }
        printf("\n");
    }

    return 0;
}