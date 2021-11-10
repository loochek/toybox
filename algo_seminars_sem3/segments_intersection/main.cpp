#include <cmath>
#include <cstdio>

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

/// Two-component long long vector
typedef Vec2<long long> Vec2i;

bool check1D(long long p11, long long p12, long long p21, long long p22)
{
    if (p11 > p12)
    {
        long long tmp = p11;
        p11 = p12;
        p12 = tmp;
    }

    if (p21 > p22)
    {
        long long tmp = p21;
        p21 = p22;
        p22 = tmp;
    }

    if (p11 <= p21 && p11 <= p22 && p21 <= p12 && p22 <= p12)
        return true;

    if (p21 <= p11 && p21 <= p12 && p11 <= p22 && p12 <= p22)
        return true;

    if (p11 <= p21 && p21 <= p12 && p12 <= p22)
        return true;

    if (p21 <= p11 && p11 <= p22 && p22 <= p12)
        return true;

    return false;
}

bool check(Vec2i p11, Vec2i p12, Vec2i p21, Vec2i p22)
{
    // check from p11
    Vec2i p11_p12 = p12 - p11;
    Vec2i p11_p21 = p21 - p11;
    Vec2i p11_p22 = p22 - p12;

    long long res1 = p11_p12 & p11_p21;
    long long res2 = p11_p12 & p11_p22;

    if ((res1 < 0 && res2 < 0) || (res1 > 0 && res2 > 0))
        return false;

    // check from p12
    Vec2i p12_p11 = p11 - p12;
    Vec2i p12_p21 = p21 - p12;
    Vec2i p12_p22 = p22 - p12;

    res1 = p12_p11 & p12_p21;
    res2 = p12_p11 & p12_p22;

    if ((res1 < 0 && res2 < 0) || (res1 > 0 && res2 > 0))
        return false;

    // check from p21
    Vec2i p21_p22 = p22 - p21;
    Vec2i p21_p11 = p11 - p21;
    Vec2i p21_p12 = p12 - p21;

    res1 = p21_p22 & p21_p11;
    res2 = p21_p22 & p21_p12;

    if ((res1 < 0 && res2 < 0) || (res1 > 0 && res2 > 0))
        return false;

    // check from p22
    Vec2i p22_p21 = p21 - p22;
    Vec2i p22_p11 = p11 - p22;
    Vec2i p22_p12 = p12 - p22;

    res1 = p22_p21 & p22_p11;
    res2 = p22_p21 & p22_p12;

    if ((res1 < 0 && res2 < 0) || (res1 > 0 && res2 > 0))
        return false;

    // kostyl number 1: if segments are on the same line

    if ((p11_p12 & p11_p21) == 0 && (p11_p12 & p11_p22) == 0)
        return check1D(p11.x, p12.x, p21.x, p22.x) && check1D(p11.y, p12.y, p21.y, p22.y);

    return true;
}

int main()
{
    Vec2i p11, p12, p21, p22;
    scanf("%lld %lld %lld %lld %lld %lld %lld %lld", &p11.x, &p11.y, &p12.x, &p12.y, &p21.x, &p21.y, &p22.x, &p22.y);

    if (check(p11, p12, p21, p22))
        printf("YES\n");
    else
        printf("NO\n");

}