#include <stdio.h>
#include <assert.h>

#include "custom_strcmp.h"

// английские буквы

void unit_test_1()
{
    const char *a = "\\\\\\......34153425A2345l3425p55h5a5"; // Alpha
    const char b[] = "2435B3425253e№;:?t234a234ы"; // Beta
    assert(custom_strcmp(a, b) < 0);
    assert(custom_strcmp(b, a) > 0);
}

void unit_test_2()
{
    const char *a = "////////\\\\\\@#$^&*A34522435l3p3h3a333"; // Alpha
    const char *b = "---\"^^^^^^A^l^p^h^a^"; // Alpha
    assert(custom_strcmp(a, b) == 0);
}

void unit_test_3()
{
    const char *a = "345C354h354a345r354l345i345e"; // Charlie
    const char *b = "B555555555eta"; // Beta
    assert(custom_strcmp(a, b) > 0);
    assert(custom_strcmp(b, a) < 0);
}

// русские буквы (заданы escape-последовательностями, не знаю как иначе)

void unit_test_4()
{
    const char *a = "////////////\\\\\\-----1\xc0\1\xeb\1\xfc\1\xf4\1\xe0"; // Альфа 
    const char *b = "----\xc1\xe5\xf2\xe0"; // Бета
    assert(custom_strcmp(a, b) < 0);
    assert(custom_strcmp(b, a) > 0);
}

void unit_test_5()
{
    // ЛоЛКеК
    const char *a = "2020202020\xcb----\xee$%\"^&\xcb\\324\xca\234\xe5\234\3\xca\2020";
    // ЛоЛКеК
    const char *b = "1111----//////\\\\\\---\xcb---\xee----\xcb---\xca--\xe5--\xca---";
    assert(custom_strcmp(a, b) == 0);
}

#ifdef TEST
int main()
#else
int dummy()
#endif
{
    unit_test_1();
    unit_test_2();
    unit_test_3();
    unit_test_4();
    unit_test_5();
    printf("Testing done, no errors!\n");
    return 0;
}