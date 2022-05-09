#include <iostream>
#include <cassert>
#include "function.hpp"

int TestFunction(int a, int b) noexcept
{
    return a + b;
}

class TestClass
{
public:
    int TestMethodConst(int x) const noexcept
    {
        return x;
    }

    int TestMethod(int x) noexcept
    {
        return x;
    }
};

class TestFunctor
{
public:
    TestFunctor() = default;

    TestFunctor(const TestFunctor& /*other*/)
    {
        std::cout << "TestFunctor copy\n";
    }

    TestFunctor(TestFunctor&& /*other*/)
    {
        std::cout << "TestFunctor move\n";
    }

    int operator()(int x) const noexcept
    {
        return x;
    }
};

int main()
{
    // 1) Free function

    Function test_func = TestFunction;
    Function test_func_copy = test_func;
    test_func = std::move(test_func_copy);
    assert(test_func(1, 2) == 3);

    // 2) Functor

    Function functor = TestFunctor();
    Function functor_copy = functor;
    functor = std::move(functor_copy);
    assert(functor(6) == 6);

    // 3) Functor 2

    int c = 10;
    Function lambda = [c](int a, int b) noexcept {
        return a + b + c;
    };

    Function lambda_copy = lambda;
    lambda = std::move(lambda_copy);
    assert(lambda(20, 30) == 60);

    // 4) Class method

    Function method(&TestClass::TestMethod);
    Function<int(const TestClass&, int) noexcept> const_method(&TestClass::TestMethodConst);

    TestClass test_class_lval;
    assert(method(test_class_lval, 1) == 1);
    assert(const_method(TestClass(), 2) == 2);

    return 0;
}
