#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>

const int INF = 1e10;
const double EPS = 1e-3;

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

template<>
bool Vec2<double>::operator==(const Vec2<double> &other) const
{
    return fabs(x - other.x) < EPS && fabs(y - other.y) < EPS;
}

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2f;

bool fless(double a, double b)
{
    return a < b - EPS;
}

bool fmore(double a, double b)
{
    return a + EPS > b;
}

bool feq(double a, double b)
{
    return fabs(a - b) < EPS;
}

int main()
{
    int n = 0;
    scanf("%d", &n);

    std::vector<std::pair<Vec2f, int>> points(n);

    // 1. Convex hull

    Vec2f start_p(INF, INF);

    for (int i = 0; i < n; i++)
    {
        Vec2f p;
        scanf("%lf %lf", &p.x, &p.y);
        points[i] = std::make_pair(p, i);

        if (fless(p.y, start_p.y) || (feq(p.y, start_p.y) && fmore(p.x, start_p.x)))
            start_p = p;
    }

    std::sort(points.begin(), points.end(), [start_p](const std::pair<Vec2f, int> &ap, const std::pair<Vec2f, int> &bp)
    {
        auto a = ap.first;
        auto b = bp.first;
        if (feq((a - start_p) & (b - start_p), 0.0))
            return fless((a - start_p) ^ (a - start_p), (b - start_p) ^ (b - start_p));

        return fmore((a - start_p) & (b - start_p), 0.0);
    });

    std::vector<std::pair<Vec2f, int>> convex;
    convex.push_back(points[0]);
    convex.push_back(points[1]);

    for (int i = 2; i < n; i++)
    {
        while (true)
        {
            int convex_sz = convex.size();
            if (convex_sz < 2)
                break;

            Vec2f vec1 = points[i].first - convex[convex_sz - 1].first;
            Vec2f vec2 = convex[convex_sz - 1].first - convex[convex_sz - 2].first;

            if (fless(vec1 & vec2, 0.0))
                break;

            convex.pop_back();
        }

        convex.push_back(points[i]);
    }

    // 2. Diameter

    int l = 0, r = 0;

    for (int i = 0; i < convex.size(); i++)
    {
        Vec2f vl = convex[l].first;
        Vec2f vr = convex[r].first;
        Vec2f vi = convex[i].first;

        if (fless(vi.x, vl.x) || feq(vi.x, vl.x) && fmore(vi.y, vl.y))
            l = i;

        if (fmore(vi.x, vr.x) || feq(vi.x, vr.x) && fless(vi.y, vr.y))
            r = i;
    }

    int ans_l = convex.size() - 1, ans_r = convex.size() - 1;

    for (int i = 0; i < 10 * n; i++)
    {
        Vec2f lr = convex[l].first - convex[r].first;
        Vec2f ans_lr = convex[ans_l].first - convex[ans_r].first;
        if (fmore(lr ^ lr, ans_lr ^ ans_lr))
        {
            ans_l = l;
            ans_r = r;
        }
        else if (feq(lr ^ lr, ans_lr ^ ans_lr))
        {
            if (std::min(convex[l].second, convex[r].second) < std::min(convex[ans_l].second, convex[ans_r].second))
            {
                ans_l = l;
                ans_r = r;
            }
        }

        int ln = (l + 1) % convex.size(), rn = (r + 1) % convex.size();

        Vec2f vec1 = convex[ln].first - convex[l].first, vec2 = convex[rn].first - convex[r].first;

        if (fless(vec1 & vec2, 0.0))
            l = ln;
        else
            r = rn;
    }

    printf("%d %d\n", std::min(convex[ans_l].second, convex[ans_r].second), std::max(convex[ans_l].second, convex[ans_r].second));
    return 0;
}