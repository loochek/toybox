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
    str += 'S';
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
    ConstStringView<char> view2 = str_copy2.View();
    str_copy2[3] = 'A';
    assert(strcmp(str_copy2.CStr(), "AMOA") == 0);
    assert(strcmp(view2.CStr(), "AMOA") == 0);
}

void test2()
{
    char buffer[21];
    MutableStringView<char> view(buffer, 21);

    const char *str = "AMOGUSAMOGUSAMOGUSAM";
    for (int i = 0; i < 20; i++)
        view.PushBack(str[i]);

    assert(strcmp(view.CStr(), "AMOGUSAMOGUSAMOGUSAM") == 0);

    Front(view) = 'Z';
    assert(strcmp(view.CStr(), "ZMOGUSAMOGUSAMOGUSAM") == 0);

    buffer[0] = 'A';
    assert(strcmp(view.CStr(), "AMOGUSAMOGUSAMOGUSAM") == 0);
}

void test3()
{
    String str = "AMOGUSAMOGUSAMOGUS";

    char buffer[21];
    MutableStringView<char> view(buffer, 21);
    view = str.CStr();
    
    std::sort(Begin(str), End(str));
    assert(strcmp(str.CStr(), "AAAGGGMMMOOOSSSUUU") == 0);

    assert(strcmp(view.CStr(), "AMOGUSAMOGUSAMOGUS") == 0);
    std::sort(Begin(view), End(view));
    assert(strcmp(view.CStr(), "AAAGGGMMMOOOSSSUUU") == 0);

    const String str2 = "AAAGGGMMMOOOSSSUUU";
    auto iter = std::find(Begin(str2), End(str2), 'U');
    assert(iter - Begin(str2) == 15);
}

int main()
{
    test1();
    test2();
    test3();

    std::cout << "All tests passed (ﾉ^_^)ﾉ\n";
    return 0;
}
