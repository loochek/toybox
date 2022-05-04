#include <iostream>
#include "function.hpp"

int main()
{
    Function<void()> kek = []() {
        std::cout << "kek\n";
    };
    
    kek();
    return 0;
}
