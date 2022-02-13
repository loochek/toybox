#include <cassert>
#include "TrackedInt.hpp"

void binSearch()
{
    FUNC_ENTRY;
    
    INT(l, 1);
    INT(r, 101);
    INT(c, 27);

    while (r - l > TrackedInt(1))
    {
        TrackedInt m = (l + r) / TrackedInt(2);

        if (m <= c)
            l = m;
        else
            r = m;
    }
}

// TrackedInt test(, TrackedInt c)
// {
//     FUNC_ENTRY;
//     return a + b + c;
// }

// TrackedInt testRef(TrackedInt &a, TrackedInt &b, TrackedInt &c)
// {
//     FUNC_ENTRY;
//     return a * (b - c);
// }

int main()
{
    FUNC_ENTRY;
    binSearch();

    // INT(res1, test(a, b, c));
    // INT(res2, testRef(a, b, c));

    return 0;
}