#include <cstring>
#include <iostream>
#include "StringView.hpp"
#include "String.hpp"

int main()
{
    char buffer[21] = {0};
    // StringView view(buffer, 20);
    String str;
    // TODO: deal with it
    // str = "amogus";
    Front(str) = 'z';
    std::cout << str.CStr() << '\n';
    return 0;
}
