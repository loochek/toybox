#include <cstdio>

template<int DIM>
class MathVec
{
public:
    template<typename... T>
    MathVec(T... values) : coords {values...}
    {
        static_assert(sizeof...(values) == DIM, "MathVec wrong initialization size");
    }

    const int operator*(const MathVec<DIM> &other) const
    {
        return DotHelper<DIM, DIM-1>::dot(*this, other);
    }

    const int operator[](size_t index) const
    {
        return coords[index];
    }

private:
    int coords[DIM];

private:
    template<int DIM_, int COORD>
    class DotHelper
    {
    public:
        static int dot(const MathVec<DIM_> &a, const MathVec<DIM_> &b)
        {
            return a[COORD] * b[COORD] + DotHelper<DIM_, COORD-1>::dot(a, b);
        }
    };

    template<int DIM_>
    class DotHelper<DIM_, 0>
    {
    public:
        static int dot(const MathVec<DIM_> &a, const MathVec<DIM_> &b)
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
