#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>

const long long INF = 1e9;

/**
 * Two-component vector
 */
template<typename T>
class Vec2
{
public:
    /// Initializes a zero vector
    Vec2() : x(0), y(0) {};
    Vec2(T x, T y) : x(x), y(y) {};

    /// Convert constructor
    template <typename U>
    Vec2(const Vec2<U> &other) : x(other.x), y(other.y) {};

    bool operator==(const Vec2 &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2 &other) const
    {
        return x != other.x || y != other.y;
    }

    Vec2 operator+(const Vec2 &other) const
    {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-() const
    {
        return Vec2(-x, -y);
    }

    Vec2 operator-(const Vec2 &other) const
    {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(T multiplier) const
    {
        return Vec2(x * multiplier, y * multiplier);
    }

    Vec2 operator/(T divider) const
    {
        return Vec2(x / divider, y / divider);
    }

    // Per-component multiplication
    Vec2 operator*(const Vec2 &other) const
    {
        return Vec2(x * other.x, y * other.y);
    }

    // Per-component division
    Vec2 operator/(const Vec2 &other) const
    {
        return Vec2(x / other.x, y / other.y);
    }

    /// Scalar multiplication
    T operator^(const Vec2 &other) const
    {
        return x * other.x + y * other.y;
    }

    T operator&(const Vec2 &other) const
    {
        return x * other.y - other.x * y;
    }

    Vec2 normalized() const
    {
        if (x == 0 && y == 0)
            return Vec2();

        double len = length();
        return *this / len;
    }

    T length() const
    {
        return sqrt(*this ^ *this);
    }

    Vec2 reflected(Vec2 normal) const
    {
        return *this - normal * (2 * (*this ^ normal));
    }

    Vec2 rotated(float angle) const
    {
        float s = sin(angle), c = cos(angle);

        return Vec2(x * c - y * s, x * s + y * c);
    }

    Vec2 &operator+=(const Vec2 &other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vec2 &operator-=(const Vec2 &other)
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    Vec2 &operator*=(T multiplier)
    {
        x *= multiplier;
        y *= multiplier;

        return *this;
    }

    Vec2 &operator/=(T divider)
    {
        x /= divider;
        y /= divider;

        return *this;
    }
    
    T x;
    T y;
};

typedef Vec2<long long> Vec2i;
typedef Vec2<double> Vec2f;

int main()
{
    long long n = 0;
    scanf("%lld", &n);

    std::vector<Vec2i> points(n);

    Vec2i start_p(INF, INF);

    for (long long i = 0; i < n; i++)
    {
        Vec2i p;
        scanf("%lld %lld", &p.x, &p.y);
        points[i] = p;

        if (p.y < start_p.y || (p.y == start_p.y && p.x > start_p.x))
            start_p = p;
    }

    std::sort(points.begin(), points.end(), [start_p](const Vec2i &a, const Vec2i &b)
    {
        if (((a - start_p) & (b - start_p)) == 0)
            return ((a - start_p) ^ (a - start_p)) < ((b - start_p) ^ (b - start_p));

        return ((a - start_p) & (b - start_p)) > 0;
    });

    std::vector<Vec2i> ans;
    ans.push_back(points[0]);
    ans.push_back(points[1]);

    for (long long i = 2; i < n; i++)
    {
        while (true)
        {
            long long ans_sz = ans.size();
            if (ans_sz < 2)
                break;

            Vec2i vec1 = points[i] - ans[ans_sz - 1], vec2 = ans[ans_sz - 1] - ans[ans_sz - 2];

            if ((vec1 & vec2) < 0)
                break;

            ans.pop_back();
        }

        ans.push_back(points[i]);
    }

    double perim = Vec2f(ans[ans.size() - 1] - ans[0]).length();
    for (long long i = 1; i < ans.size(); i++)
        perim += Vec2f(ans[i] - ans[i - 1]).length();

    printf("%.2f\n", perim);
    return 0;
}