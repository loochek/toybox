#include <cstring>
#include <iostream>
#include "StringView.hpp"

int main()
{
    char buffer[21] = {0};
    StringView view(buffer, 20);
    view = "amogus";
    Front(view) = 'z';
    std::cout << view.CStr() << '\n';
    return 0;
}
