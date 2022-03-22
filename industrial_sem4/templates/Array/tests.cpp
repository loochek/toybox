#include <iostream>
#include <cassert>
#include "Array.hpp"

void test1()
{
    Array<Array<int, StaticStorage, 10>> arr;

    for (int i = 0; i < 100; i++)
    {
        arr.EmplaceBack();
        for (int j = 0; j < 10; j++)
            arr.Back()[j] = i + j;
    }

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < arr[i].Size(); j++)
            assert(arr[i][j] == i + j);
    }
}

void test2()
{
    Array<Array<int>> arr(100);

    for (int i = 0; i < 100; i++)
    {
        arr[i].Resize(100);
        for (int j = 0; j < 100; j++)
            arr[i][j] = i + j;
    }

    for (int i = 0; i < arr.Size(); i++)
    {
        for (int j = 0; j < arr[i].Size(); j++)
            assert(arr[i][j] == i + j);
    }
}

void test3()
{
    Array<Array<int, StaticStorage, 10>> arr;

    for (int i = 0; i < 100; i++)
    {
        arr.EmplaceBack();
        for (int j = 0; j < 10; j++)
            arr.Back()[j] = i + j;
    }

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < arr[i].Size(); j++)
            assert(arr[i][j] == i + j);
    }
}

void test4()
{
    Array<int> arr(4096);

    for (int j = 0; j < 128; j++)
    {
        for (int i = 0; i < arr.Size(); i++)
            arr[i] = i;

        while (!arr.Empty())
        {
            for (int i = 0; i < 128; i++)
                arr.PopBack();

            arr.Shrink();
        }

        arr.Resize(4096);
    }
}

int main()
{
    test1();
    test2();
    test3();
    test4();

    std::cout << "All tests passed (ﾉ^_^)ﾉ\n";
}
