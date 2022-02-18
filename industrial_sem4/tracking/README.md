# TrackedInt

There is a tool that hooks operations with instances of a certain class that behaves like an integer. The result is a text and graphic log that represents the full lifecycle of objects - creation, copying, moving and destruction. To demonstrate how the tool works, I propose to analyze the lifetime of temporary objects and how move semantics allow us to write more efficient code.

## Analysis

Let's take a look to this sample program:

```c++
TrackedInt test(TrackedInt a, TrackedInt b, TrackedInt c)
{
    FUNC_ENTRY;
    return a + b + c;
}

void testEntry()
{
    FUNC_ENTRY;

    INT(res1, test(TrackedInt(10), TrackedInt(20), TrackedInt(30)));

    INT(a, 0);
    INT(b, 0);
    INT(c, 0);

    a = 1;
    b = 2;
    c = 3;

    INT(res2, test(a, b, c));

    INT(res3, 0);
    res3 = (a - b) * c;
}
```

There are several additional macros INT and FUNC_ENTRY to inform the tool about the names of the objects being created and the function calls, respectively. So, let's take a look to the tool's output (move semantics are not used):

<pre>
void testEntry()
{
    [New] "temp object 0" (0|0x7ffd59cf6ae0) <-- 10
    [New] "temp object 1" (1|0x7ffd59cf6a98) <-- 20
    [New] "temp object 2" (2|0x7ffd59cf6a50) <-- 30
    TrackedInt test(TrackedInt, TrackedInt, TrackedInt)
    {
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 3" (3|0x7ffd59cf6500) <--(30)-- "temp object 0" (0|0x7ffd59cf6ae0) + "temp object 1" (1|0x7ffd59cf6a98)
        }
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 4" (4|0x7ffd59cf6b08) <--(60)-- "temp object 3" (3|0x7ffd59cf6500) + "temp object 2" (2|0x7ffd59cf6a50)
        }
        [EOL] "temp object 3" (3|0x7ffd59cf6500)
    }
    [<font color=#FF0000>COPY</font>] "res1" (5|0x7ffd59cf6b30) <--(60)-- "temp object 4" (4|0x7ffd59cf6b08)
    [EOL] "temp object 4" (4|0x7ffd59cf6b08)
    [EOL] "temp object 2" (2|0x7ffd59cf6a50)
    [EOL] "temp object 1" (1|0x7ffd59cf6a98)
    [EOL] "temp object 0" (0|0x7ffd59cf6ae0)
    [New] "a" (6|0x7ffd59cf69e8) <-- 0
    [New] "b" (7|0x7ffd59cf69a0) <-- 0
    [New] "c" (8|0x7ffd59cf6958) <-- 0
    [New] "temp object 9" (9|0x7ffd59cf6910) <-- 1
    [<font color=#FF0000>Copy=</font>] "a" (6|0x7ffd59cf69e8) <--(1)-- "temp object 9" (9|0x7ffd59cf6910)
    [EOL] "temp object 9" (9|0x7ffd59cf6910)
    [New] "temp object 10" (10|0x7ffd59cf68c8) <-- 2
    [<font color=#FF0000>Copy=</font>] "b" (7|0x7ffd59cf69a0) <--(2)-- "temp object 10" (10|0x7ffd59cf68c8)
    [EOL] "temp object 10" (10|0x7ffd59cf68c8)
    [New] "temp object 11" (11|0x7ffd59cf6880) <-- 3
    [<font color=#FF0000>Copy=</font>] "c" (8|0x7ffd59cf6958) <--(3)-- "temp object 11" (11|0x7ffd59cf6880)
    [EOL] "temp object 11" (11|0x7ffd59cf6880)
    [<font color=#FF0000>COPY</font>] "temp object 12" (12|0x7ffd59cf67e8) <--(1)-- "a" (6|0x7ffd59cf69e8)
    [<font color=#FF0000>COPY</font>] "temp object 13" (13|0x7ffd59cf67a0) <--(2)-- "b" (7|0x7ffd59cf69a0)
    [<font color=#FF0000>COPY</font>] "temp object 14" (14|0x7ffd59cf6758) <--(3)-- "c" (8|0x7ffd59cf6958)
    TrackedInt test(TrackedInt, TrackedInt, TrackedInt)
    {
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 15" (15|0x7ffd59cf6500) <--(3)-- "temp object 12" (12|0x7ffd59cf67e8) + "temp object 13" (13|0x7ffd59cf67a0)
        }
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 16" (16|0x7ffd59cf6810) <--(6)-- "temp object 15" (15|0x7ffd59cf6500) + "temp object 14" (14|0x7ffd59cf6758)
        }
        [EOL] "temp object 15" (15|0x7ffd59cf6500)
    }
    [<font color=#FF0000>COPY</font>] "res2" (17|0x7ffd59cf6838) <--(6)-- "temp object 16" (16|0x7ffd59cf6810)
    [EOL] "temp object 16" (16|0x7ffd59cf6810)
    [EOL] "temp object 14" (14|0x7ffd59cf6758)
    [EOL] "temp object 13" (13|0x7ffd59cf67a0)
    [EOL] "temp object 12" (12|0x7ffd59cf67e8)
    [New] "res3" (18|0x7ffd59cf66f0) <-- 0
    TrackedInt TrackedInt::operator-(const TrackedInt &) const
    {
        [New] "temp object 19" (19|0x7ffd59cf6680) <--(-1)-- "a" (6|0x7ffd59cf69e8) - "b" (7|0x7ffd59cf69a0)
    }
    TrackedInt TrackedInt::operator*(const TrackedInt &) const
    {
        [New] "temp object 20" (20|0x7ffd59cf66a8) <--(-3)-- "temp object 19" (19|0x7ffd59cf6680) * "c" (8|0x7ffd59cf6958)
    }
    [<font color=#FF0000>Copy=</font>] "res3" (18|0x7ffd59cf66f0) <--(-3)-- "temp object 20" (20|0x7ffd59cf66a8)
    [EOL] "temp object 20" (20|0x7ffd59cf66a8)
    [EOL] "temp object 19" (19|0x7ffd59cf6680)
    [EOL] "res3" (18|0x7ffd59cf66f0)
    [EOL] "res2" (17|0x7ffd59cf6838)
    [EOL] "c" (8|0x7ffd59cf6958)
    [EOL] "b" (7|0x7ffd59cf69a0)
    [EOL] "a" (6|0x7ffd59cf69e8)
    [EOL] "res1" (5|0x7ffd59cf6b30)
}
Total: 15 tmp object, 9 copies
</pre>

The tool tries to show variable names that it knows about. However, each object has a unique number that allows to distinguish between temporary objects, as well as objects with the same name due to nested function calls. The object address is also displayed along with the unique number.

Let's return to our program. We can see that some temporary objects are deleted shortly after.