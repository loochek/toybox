# Move semantics - advanced

## std::move

In the previous article, we managed to optimize unnecessary copying of temporary objects. But there are situations when we, as programmers, see that same can be done with with non-temporary objects:

```c++
TrackedInt square(TrackedInt arg)
{
    return arg * arg;
}

void testEntry()
{
    INT(a, 10);
    INT(result, square(a));
    // a is not used anymore
}
```

Of course, compiler can't move `a` automatically - we must tell the compiler that test is not used and it's content can be "stolen" instead of copying. Due to C++ semantics, it can be done with:

```c++
INT(a, 10);
INT(result, static_cast<TrackedInt&&>(square(a)));
// a is can't be used anymore
```

C++ standart library offers us `std::move` - special function that casts anything to rvalue reference. It's implementation looks something like this:

```c++
template<typename T>
typename std::remove_reference<T>::type &&move(T &&arg)
{
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}
```

Due to template type deduction rules and reference collapsing, it can be passed with both lvalues and rvalues. `std::remove_reference` is a special class that - suddenly - removes reference from type T.

So, previous example can be written like this
```c++
INT(a, 10);
INT(result, square(std::move(a)));
// a is can't be used anymore
```

Let's make sure the move works:

| Without move                    | With move                    |
| ------------------------------  | ---------------------------- |
| ![](../images/without_move.png) | ![](../images/with_move.png) |

Just like we want - object is moved instead of copying.

## std::forward

Sometimes we want to pass value to inner function - and so that rvalue is moved and lvalue is copied. For example, let's look at a function which creates the class instance with some given argument. In order to pass both lvalues and rvalues, we use an universal reference.

```c++
template<typename T, typename Arg>
T *createDynamic(Arg &&arg)
{
    // case 1
    return new T(arg);
    // case 2
    return new T(std::move(arg));
    // case 3
    return new T(std::forward<Arg>(arg));
}
```

There are a test program:

```c++
void testEntry()
{
    INT(lval, 10);

    TrackedInt *lvalue_copy = createDynamic<TrackedInt>(lval);
    TrackedInt *rvalue_copy = createDynamic<TrackedInt>(TrackedInt(10));
}
```

| Without anything                | Move                                    | Forward                         |
| ------------------------------  | --------------------------------------- | ------------------------------- |
| ![](../images/without_move.png) | ![](../images/move_instead_forward.png) | ![](../images/with_forward.png) |

Case 1 doesn't work as intented. Rvalue is not perfectly forwarded - it's copied inside `createDynamic`. This is because `arg` is lvalue - despite it's type.

Maybe use `move` (case 2)? It's a bad idea - lvalue which passed to `createDynamic` will be forcely moved in this case.

Notice that we have `Arg` template parameter - it unfolds to `T` if rvalue is passed and to `T&` if lvalue is passed. So, we want to get a function, which would get an argument of type `A` and return `A&&` in the first case and `A&` in the second. It is what `std::forward` does in case 3. With forward, the test program works as intented - rvalue is moved, lvalue is moved:

`std::forward`'s implementation looks something like this:

```c++
template<typename T>
T&& forward(T &arg)
{
    return static_cast<T&&>(arg);
}
```

The main point is `T&` as argument. In any case, argument is unfolded to `A&`. Cast type and return value type are unfolded like this - wheh `T` is `A` (it's true when rvalue is passed), `T&&` is `A&&`, and when `T` is `A&` (it's true when lvalue is passed), `T&&` is `A&&`. Anything works as we want.
