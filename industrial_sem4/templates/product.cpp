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

    const int operator*(const MathVec<N> &other) const
    {
        return DotHelper<N, N-1>::dot(*this, other);
    }

    const int operator[](size_t index) const
    {
        return coords[index];
    }

private:
    int coords[N];

private:
    template<int N1, int COORD>
    class DotHelper
    {
    public:
        static int dot(const MathVec<N1> &a, const MathVec<N1> &b)
        {
            return a[COORD] * b[COORD] + DotHelper<N1, COORD-1>::dot(a, b);
        }
    };

    template<int N1>
    class DotHelper<N1, 0>
    {
    public:
        static int dot(const MathVec<N1> &a, const MathVec<N1> &b)
        {
            return a[0] * b[0];
        }
    };
};

int main()
{
    int x = 0, y = 0, z = 0;
    scanf("%d %d %d", &x, &y, &z);

    MathVec<3> a(1, 2, 3);
    MathVec<3> b(x, y, z);

    printf("%d\n", a * b);
}
