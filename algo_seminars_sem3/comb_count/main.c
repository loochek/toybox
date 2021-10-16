#include <stdio.h>
#include <stdlib.h>

#define int long long

const int MOD = 1000000007;

const int MAX_FRAC = 1000000;

int binpow(int a, int n)
{
	int res = 1;
	while (n)
    {
        if (n & 1)
        {
			res = (res * a) % MOD;
			n--;
		}
		else
        {
			a = (a * a) % MOD;
			n >>= 1;
		}
    }	
	return res;
}

int main()
{
    int *straigh_fracs = calloc(MAX_FRAC + 1, sizeof(int));
    straigh_fracs[0] = 1;
    straigh_fracs[1] = 1;
    for (int i = 2; i <= MAX_FRAC; i++)
        straigh_fracs[i] = (straigh_fracs[i - 1] * i) % MOD;

    int *reverse_fracs = calloc(MAX_FRAC + 1, sizeof(int));
    reverse_fracs[MAX_FRAC] = binpow(straigh_fracs[MAX_FRAC], MOD - 2);

    for (int i = MAX_FRAC - 1; i >= 0; i--)
        reverse_fracs[i] = (reverse_fracs[i + 1] * (i + 1)) % MOD;

    int n = 0, m = 0, l = 0;
    scanf("%lld %lld %lld", &n, &m, &l);

    int ans = 0;

    for (int k = 1; k <= l; k++)
    {
        if (m * k > n)
            break;

        ans = (ans + ((straigh_fracs[n] * reverse_fracs[n - m * k]) % MOD * reverse_fracs[m * k]) % MOD) % MOD;
    }

    printf("%lld\n", ans);

    free(straigh_fracs);
    free(reverse_fracs);
    return 0;
}