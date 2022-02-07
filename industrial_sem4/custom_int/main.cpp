#include <cassert>
#include "CustomInt.hpp"

int main()
{
    CustomInt a = 1, b = 2, c = 3;
    CustomInt aCopy = a, bCopy = b, cCopy = c;
    
    CustomInt sum = a + b + c;

    assert(sum > a && sum > b && sum > c);
}