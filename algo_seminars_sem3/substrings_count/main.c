#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_STR_LEN 10000

int main()
{
    char *str = calloc(MAX_STR_LEN + 1, sizeof(char));
    scanf("%s", str);

    int n = strlen(str);

    for (int i = 0; i < n / 2; i++)
    {
        char c = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = c;
    }

    int ans = 0;

    int *z_func = calloc(n, sizeof(int));
    for (int j = n - 1; j >= 0; j--)
    {
        memset(z_func, 0, sizeof(int) * n);

        int subs_len = n - j;

        int l = 0, r = 0;
        int z_max = 0;
        for (int i = 1; i < subs_len; i++)
        {
            if (i <= r)
            {
                z_func[i] = r - i + 1;
                if (z_func[i - l] < z_func[i])
                    z_func[i] = z_func[i - l];
            }
            while (i + z_func[i] < n && str[z_func[i] + j] == str[i + z_func[i] + j])
                z_func[i]++;
            if (i + z_func[i] - 1 > r)
            {
                l = i;
                r = i + z_func[i] - 1;
            }

            if (z_func[i] > z_max)
                z_max = z_func[i];
        }

        ans += subs_len - z_max;
    }

    printf("%d\n", ans);

    free(z_func);
    free(str);
    return 0;
}