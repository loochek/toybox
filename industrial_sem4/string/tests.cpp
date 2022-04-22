#include <cstring>
#include <iostream>
#include "StringView.hpp"
#include "String.hpp"

void test1()
{
    String str;
    str = "AMOGUS";
    assert(strcmp(str.CStr(), "AMOGUS") == 0);

    str.PushBack('S');
    str.PushBack('S');
    assert(strcmp(str.CStr(), "AMOGUSSS") == 0);

    str.PopBack();
    str.PopBack();
    str.PopBack();
    str.PopBack();
    assert(strcmp(str.CStr(), "AMOG") == 0);

    str.Shrink(); // Does nothing

    const String str_copy = str;
    assert(strcmp(str_copy.CStr(), "AMOG") == 0);

    String str_copy2 = str;
    str_copy2[3] = 'A';
    assert(strcmp(str_copy2.CStr(), "AMOA") == 0);
}

void test2()
{
    char buffer[21];
    StringView view(buffer, 21);

    const char *str = "AMOGUSAMOGUSAMOGUSAM";
    for (int i = 0; i < 20; i++)
        view.PushBack(str[i]);

    assert(strcmp(view.CStr(), "AMOGUSAMOGUSAMOGUSAM") == 0);

    const StringView view_copy = view;
    Front(view) = 'Z';
    assert(strcmp(view_copy.CStr(), "ZMOGUSAMOGUSAMOGUSAM") == 0);
}

int main()
{
    test1();
    test2();

    std::cout << "All tests passed (ﾉ^_^)ﾉ\n";
    return 0;
}
