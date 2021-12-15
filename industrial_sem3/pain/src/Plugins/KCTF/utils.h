#include <cmath>

template <typename T, int neutral, int T_EPS>
struct Vec2 {
    static constexpr T EPS = T_EPS / 1000000;

    union {
        struct {
            T x;
            T y;
        };

        T data[2];
    };

    Vec2() : x(neutral), y(neutral) {}
    Vec2(const T &x, const T &y) : x(x), y(y) {}
    Vec2(const T &fill) : x(fill), y(fill) {}

    inline T len() const {
        return hypot(x, y);
    }

    inline T len_squared() const {
        return x * x + y * y;
    }

    inline Vec2 normal() const {
        T l = len();
        if(l < EPS) {
            return {neutral, neutral};
        } else {
            return {x / l, y / l};
        }
    }

    inline void normalize() {
        T l = len();
        if (l < EPS) { x = neutral; y = neutral; }
        else { x /= l; y /= l; }
    }

    inline bool is_zero() const {
        return len() < EPS;
    }

    inline T dot(const Vec2 &other) const {
        return x * other.x + y * other.y;
    }

    inline T cross(const Vec2 &other) const {
        return x * other.y - y * other.x;
    }

    double operator[](const int i) const {
        return data[i];
    }

    inline Vec2 rot90(int dir = 1) const {
        return {(-1 * dir) * x, (1 * dir) * y};
    }
};


template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator+(const Vec2<T, neutral, eps> &first) {
    return first;
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator-(const Vec2<T, neutral, eps> &first) {
    return {-1 * first.x, -1 * first.y};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator+(const Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    return {first.x + second.x, first.y + second.y};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator-(const Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    return {first.x - second.x, first.y - second.y};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator*(const Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    return {first.x * second.x, first.y * second.y};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator/(const Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    return {first.x / second.x, first.y / second.y};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator*(const Vec2<T, neutral, eps> &first, const double k) {
    return {static_cast<T>(k * first.x), static_cast<T>(k * first.y)};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator*(const double k, const Vec2<T, neutral, eps> &first) {
    return {static_cast<T>(k * first.x), static_cast<T>(k * first.y)};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> operator/(const Vec2<T, neutral, eps> &first, const double k) {
    return {static_cast<T>(first.x / k), static_cast<T>(first.y / k)};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> &operator+=(Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    first.x += second.x;
    first.y += second.y;
    return first;
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> &operator-=(Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    first.x -= second.x;
    first.y -= second.y;
    return first;
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> &operator*=(Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    first.x *= second.x;
    first.y *= second.y;
    return first;
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> &operator/=(Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    first.x /= second.x;
    first.y /= second.y;
    return first;
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> &operator*=(Vec2<T, neutral, eps> &first, const double k) {
    first.x *= k;
    first.y *= k;
    return first;
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> &operator/=(Vec2<T, neutral, eps> &first, const double k) {
    first.x /= k;
    first.y /= k;
    return first;
}

template <typename T, int neutral, int eps>
inline bool operator==(const Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    return abs(first.x - second.x) < first.EPS && abs(first.y - second.y) < first.EPS;
}

template <typename T, int neutral, int eps>
inline bool operator<(const Vec2<T, neutral, eps> &first, const Vec2<T, neutral, eps> &second) {
    return first.x - second.x < first.EPS && first.y - second.y < first.EPS;
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> sqrt(const Vec2<T, neutral, eps> &first) {
    return {sqrt(first.x),
            sqrt(first.y)};
}

template <typename T, int neutral, int eps>
inline Vec2<T, neutral, eps> pow(const Vec2<T, neutral, eps> &first, const double power) {
    return {pow(first.x, power),
            pow(first.y, power)};
}

using Vec2i = Vec2<int,    0, 0>;
using Vec2f = Vec2<float,  0, 1>;
using Vec2d = Vec2<double, 0, 1>;
