#include <iostream>
#include "Array.hpp"

int main()
{
    Array<bool, 10> arr;

    for (int i = 0; i < 10; i++)
        arr[i] = i % 2;

    for (int i = 0; i < 10; i++)
        std::cout << arr[i] << ' ';

    std::cout << '\n';
}
