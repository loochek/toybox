#include <iostream>
#include "function.hpp"

int main()
{
    Function kek = [](int a, int b) {
        std::cout << "kek\n";
        return a + b;
    };

    std::cout << kek(1, 2) << std::endl;
    return 0;
}
