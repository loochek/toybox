// a1 = 0, d = 1
// offset of ith row (from 0) - (0 + i) * (i + 1) / 2
// 0 1 3 6

#include <stdio.h>
#include <stdlib.h>

#define TRIANGLE(r, c) ((r - 1) * r / 2 + c)

int main()
{
    FILE *fin = fopen("input.txt", "r");
    int n = 0;
    fscanf(fin, "%d", &n);
    int *arr = (int*)malloc(TRIANGLE(n, 0) * sizeof(int));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            int elem;
            fscanf(fin, "%d", &elem);
            if (j < i)
                arr[TRIANGLE(i, j)] = elem;
        }
    fclose(fin);
    // доказательство, что все хранится линейно
    // for (int i = 0; i < TRIANGLE(n, 0); i++)
    //     printf("%d ", arr[i]);
    FILE *fout = fopen("output.txt", "w");
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (j < i)
                fprintf(fout, "%2d ", arr[TRIANGLE(i, j)]);
            else
                fprintf(fout, " 0 ");
        }
        fprintf(fout, "\n");
    }
    fclose(fout);
    free(arr);
    return 0;
}