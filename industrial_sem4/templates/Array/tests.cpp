#include <iostream>
#include <cassert>
#include "Array.hpp"

void test1()
{
    Array<Array<int, StaticStorage, 10>> arr;

    for (int i = 0; i < 100; i++)
    {
        assert(arr.Size() == i);
        arr.EmplaceBack();
        assert(arr[i].Size() == 10);
        for (int j = 0; j < 10; j++)
            arr.Back()[j] = i + j;
    }

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < arr[i].Size(); j++)
            assert(arr[i][j] == i + j);
    }

    //--------------------------------------------------------

    const Array<Array<int, StaticStorage, 10>> copy = arr;
    assert(copy.Size() == 100);

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < copy[i].Size(); j++)
            assert(copy.At(i).At(j) == i + j);
    }

    //--------------------------------------------------------

    const Array<Array<int, StaticStorage, 10>> move = std::move(arr);
    assert(move.Size() == 100);
    assert(arr.Size() == 0);

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < move[i].Size(); j++)
            assert(move.At(i).At(j) == i + j);
    }

    Array<Array<int, StaticStorage, 10>> copy2 = move;
    Array<Array<int, StaticStorage, 10>> copy3 = move;
    assert(copy2.Size() == 100);
    assert(copy3.Size() == 100);

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < copy[i].Size(); j++)
        {
            assert(copy2.At(i).At(j) == i + j);
            assert(copy3.At(i).At(j) == i + j);
        }
    }

    copy2.Resize(50);
    assert(copy2.Size() == 50);
    copy2 = copy3;
    assert(copy2.Size() == 100);
    assert(copy3.Size() == 100);
    copy2 = std::move(copy3);
    assert(copy2.Size() == 100);
    assert(copy3.Size() == 0);

    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < copy[i].Size(); j++)
        {
            assert(copy2.At(i).At(j) == i + j);
        }
    }

    //--------------------------------------------------------

    copy2.Clear();
    assert(copy2.Size() == 0);
}

void test2()
{
    Array<Array<int>> arr(100);
    assert(arr.Size() == 100);

    for (int i = 0; i < 100; i++)
    {
        arr[i].Resize(100);
        assert(arr[i].Size() == 100);
        for (int j = 0; j < 100; j++)
            arr[i][j] = i + j;
    }

    for (int i = 0; i < arr.Size(); i++)
    {
        for (int j = 0; j < arr[i].Size(); j++)
            assert(arr[i][j] == i + j);
    }

    arr[37].Clear();
    assert(arr[37].Size() == 0);
    arr.Clear();
    assert(arr.Size() == 0);
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

void test5()
{
    Array<bool> arr(100);
    assert(arr.Size() == 100);
    
    for (int i = 0; i < 100; i++)
        arr[i] = i % 2;

    for (int i = 100; i < 137; i++)
    {
        assert(arr.Size() == i);
        arr.PushBack(i % 2);
    }

    for (int i = 0; i < 137; i++)
        assert(arr.At(i) == i % 2);

    arr.Clear();
    assert(arr.Size() == 0);
}

void test6()
{
    const Array<int> arr = {1, 2, 3, 4, 5};
    assert(arr.Size() == 5);

    for (int i = 0; i < 5; i++)
        assert(arr.At(i) == i + 1);

    Array<int> arr2 = arr;
    assert(arr2.Size() == 5);

    for (int i = 0; i < 5; i++)
        assert(arr2.At(i) == i + 1);

    arr2 = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    assert(arr2.Size() == 10);
    for (int i = 0; i < 10; i++)
        assert(arr2.At(i) == 11 + i);

    arr2.Clear();
    assert(arr2.Size() == 0);
}

void test7()
{
    const Array<bool> arr = {0, 1, 0, 1, 0};
    assert(arr.Size() == 5);

    for (int i = 0; i < 5; i++)
        assert(arr.At(i) == i % 2);

    Array<bool> arr2 = arr;
    assert(arr2.Size() == 5);

    for (int i = 0; i < 5; i++)
        assert(arr2.At(i) ==  i % 2);

    arr2 = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    assert(arr2.Size() == 10);
    for (int i = 0; i < 10; i++)
        assert(arr2.At(i) == (i + 1) % 2);

    arr2.Clear();
    assert(arr2.Size() == 0);
}

int main()
{
    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    test7();

    std::cout << "All tests passed (ﾉ^_^)ﾉ\n";
}
