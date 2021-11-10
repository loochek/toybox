#include <cstdio>
#include <vector>
#include <algorithm>

const long long INF = 1e14;

int main()
{
    long long n = 0;
    scanf("%lld", &n);

    std::vector<std::pair<long long, long long>> points(n);
    for (long long i = 0; i < n; i++)
    {
        long long tmp = 0;
        scanf("%lld", &tmp);
        points[i] = std::pair<long long, long long>(tmp, i);
    }
        
    std::sort(points.begin(), points.end());

    long long p1 = -1, p2 = -1, min_dist = INF;
    for (long long i = 1; i < n; i++)
    {
        if (points[i].first - points[i - 1].first < min_dist)
        {
            min_dist = points[i].first - points[i - 1].first;
            p1 = std::min(points[i - 1].second, points[i].second);
            p2 = std::max(points[i - 1].second, points[i].second);
        }
    }

    printf("%lld\n%lld %lld\n", min_dist, p1 + 1, p2 + 1);

    return 0;
}