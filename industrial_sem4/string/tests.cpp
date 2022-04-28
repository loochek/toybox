#include <cstring>
#include <iostream>
#include "StringView.hpp"
#include "String.hpp"

void test1()
{
    String str;
    str = "AMOGUS";
    assert(strcmp(str.Data(), "AMOGUS") == 0);

    str.PushBack('S');
    str += 'S';
    assert(strcmp(str.Data(), "AMOGUSSS") == 0);

    str.PopBack();
    str.PopBack();
    str.PopBack();
    str.PopBack();
    assert(strcmp(str.Data(), "AMOG") == 0);

    str.Shrink(); // Does nothing

    const String str_copy = str;
    assert(strcmp(str_copy.Data(), "AMOG") == 0);

    String str_copy2 = str;
    ConstStringView<char> view2 = str_copy2.View();
    assert(strcmp(view2.Data(), "AMOG") == 0);
    str_copy2[3] = 'A';
    assert(strcmp(str_copy2.Data(), "AMOA") == 0);
}

void test2()
{
    char buffer[21];
    MutableStringView<char> view(buffer, 21);

    const char *str = "AMOGUSAMOGUSAMOGUSAM";
    for (int i = 0; i < 20; i++)
        view.PushBack(str[i]);

    assert(strcmp(view.Data(), "AMOGUSAMOGUSAMOGUSAM") == 0);

    Front(view) = 'Z';
    assert(strcmp(view.Data(), "ZMOGUSAMOGUSAMOGUSAM") == 0);

    buffer[0] = 'A';
    assert(strcmp(view.Data(), "AMOGUSAMOGUSAMOGUSAM") == 0);
}

void test3()
{
    String str = "AMOGUSAMOGUSAMOGUS";

    char buffer[21];
    MutableStringView<char> view(buffer, 21);
    view = str.Data();
    
    std::sort(Begin(str), End(str));
    assert(strcmp(str.Data(), "AAAGGGMMMOOOSSSUUU") == 0);

    assert(strcmp(view.Data(), "AMOGUSAMOGUSAMOGUS") == 0);
    std::sort(Begin(view), End(view));
    assert(strcmp(view.Data(), "AAAGGGMMMOOOSSSUUU") == 0);

    const String str2 = view.Data();
    auto iter = std::find(Begin(str2), End(str2), 'U');
    assert(iter - Begin(str2) == 15);
}

void test4()
{
    // COW stress
    
    String str1(2);
    assert(str1.Size() == 2);
    str1 = "TestTestTest";
    assert(str1.Size() == 12);

    String str2 = str1;
    String str3 = str2;
    String str4 = str3;
    String str5 = str4;

    assert(strcmp(str1.Data(), "TestTestTest") == 0);
    assert(str1.Size() == 12);
    assert(strcmp(str2.Data(), "TestTestTest") == 0);
    assert(str2.Size() == 12);
    assert(strcmp(str3.Data(), "TestTestTest") == 0);
    assert(str3.Size() == 12);
    assert(strcmp(str4.Data(), "TestTestTest") == 0);
    assert(str4.Size() == 12);
    assert(strcmp(str5.Data(), "TestTestTest") == 0);
    assert(str5.Size() == 12);

    str2 = "OtherOtherOther";
    str4 = "AAAAAAAAAAAAAAAAAAAAAAAA";

    assert(strcmp(str1.Data(), "TestTestTest") == 0);
    assert(str1.Size() == 12);
    assert(strcmp(str2.Data(), "OtherOtherOther") == 0);
    assert(str2.Size() == 15);
    assert(strcmp(str3.Data(), "TestTestTest") == 0);
    assert(str3.Size() == 12);
    assert(strcmp(str4.Data(), "AAAAAAAAAAAAAAAAAAAAAAAA") == 0);
    assert(str4.Size() == 24);
    assert(strcmp(str5.Data(), "TestTestTest") == 0);
    assert(str5.Size() == 12);

    String str6 = str1;
    Front(str1) = 'A';
    Front(str6) = 'B';

    assert(strcmp(str1.Data(), "AestTestTest") == 0);
    assert(str1.Size() == 12);
    assert(strcmp(str2.Data(), "OtherOtherOther") == 0);
    assert(str2.Size() == 15);
    assert(strcmp(str3.Data(), "TestTestTest") == 0);
    assert(str3.Size() == 12);
    assert(strcmp(str4.Data(), "AAAAAAAAAAAAAAAAAAAAAAAA") == 0);
    assert(str4.Size() == 24);
    assert(strcmp(str5.Data(), "TestTestTest") == 0);
    assert(str5.Size() == 12);
    assert(strcmp(str6.Data(), "BestTestTest") == 0);
    assert(str6.Size() == 12);
}

int main()
{
    test1();
    test2();
    test3();
    test4();

    std::cout << "All tests passed (ﾉ^_^)ﾉ\n";
    return 0;
}
