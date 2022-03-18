#include <cstdio>

template<int N>
class MathVec
{
public:
    template<typename... T>
    MathVec(T... values) : coords {values...}
    {
        static_assert(sizeof...(values) == N, "MathVec wrong initialization size");
    }

public:
    int coords[N];
};

// template<int N>
// int scalarProduct(const MathVec<N> &a, const MathVec<N> &b)
// {
//     return 
// }

// template<typename... T>
// int scalarProduct(int a, int b, const T&... other)
// {
//     return a * b + scalarProduct(other...);
// }

template<int N, int M>
class lol
{
public:
    static int scalarProduct2Int(const MathVec<N> &a, const MathVec<N> &b)
    {
        return a.coords[M] * b.coords[M] + lol<N, M-1>::scalarProduct2Int(a, b);
    }
};

template<int N>
class lol<N, 0>
{
public:
    static int scalarProduct2Int(const MathVec<N> &a, const MathVec<N> &b)
    {
        return a.coords[0] * b.coords[0];
    }
};

template<int N>
int scalarProduct2(const MathVec<N> &a, const MathVec<N> &b)
{
    return lol<N, N-1>::scalarProduct2Int(a, b);
}

int main()
{
    int x = 0, y = 0, z = 0;
    scanf("%d %d %d", &x, &y, &z);

    MathVec<3> a(1, 2, 3);
    MathVec<3> b(x, y, z);

    printf("%d\n", scalarProduct2(a, b));
}
