#include <vector>
#include <algorithm>
#include <random>
#include "CustomInt.hpp"

int main()
{
    std::vector<CustomInt> arr(10);

    for (int i = 0; i < 10; i++)
        arr[i] = i;

    std::mt19937 rand;
    std::shuffle(arr.begin(), arr.end(), rand);
    std::sort(arr.begin(), arr.end());
}