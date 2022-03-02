# Tool overview

Let's run the tool on the simple program:

```c++
TrackedInt test(TrackedInt a, TrackedInt b, TrackedInt c)
{
    FUNC_ENTRY;
    return a + b + c;
}

void testEntry()
{
    INT(a, 1);
    INT(b, test(TrackedInt(10), TrackedInt(20), TrackedInt(30)));
    INT(c, b);
}
```

There are several additional macros INT and FUNC_ENTRY to inform the tool about the names of the objects being created and the function calls, respectively. Let's take a look to the tool's output:

<pre>
[New] "a" (0|0x7fff41297300) <-- 1
[New] "temp object 1" (1|0x7fff41297268) <-- 10
[New] "temp object 2" (2|0x7fff41297220) <-- 20
[New] "temp object 3" (3|0x7fff412971d8) <-- 30
TrackedInt test(TrackedInt, TrackedInt, TrackedInt)
{
    TrackedInt TrackedInt::operator+(const TrackedInt &) const
    {
        [New] "temp object 4" (4|0x7fff41297080) <--(30)-- "temp object 1" (1|0x7fff41297268) + "temp object 2" (2|0x7fff41297220)
    }
    TrackedInt TrackedInt::operator+(const TrackedInt &) const
    {
        [New] "temp object 5" (5|0x7fff41297290) <--(60)-- "temp object 4" (4|0x7fff41297080) + "temp object 3" (3|0x7fff412971d8)
    }
    [EOL] "temp object 4" (4|0x7fff41297080)
}
[<font color=#00FF00>Move</font>] "b" (6|0x7fff412972b8) <--(60)-- "temp object 5" (5|0x7fff41297290)
[EOL] "temp object 5" (5|0x7fff41297290)
[EOL] "temp object 3" (3|0x7fff412971d8)
[EOL] "temp object 2" (2|0x7fff41297220)
[EOL] "temp object 1" (1|0x7fff41297268)
[<font color=#FF0000>COPY</font>] "c" (7|0x7fff41297170) <--(60)-- "b" (6|0x7fff412972b8)
[EOL] "c" (7|0x7fff41297170)
[EOL] "b" (6|0x7fff412972b8)
[EOL] "a" (0|0x7fff41297300)
Total: 5 tmp object, 1 copies
</pre>

It's quite straighforward. `[New]` means that a new object is created and initialized with some value (may be result of some operator), `[Copy]` and `[Move]` represents copy and move constructors correspondingly. Similarly, `[Copy=]` and `[Move=]` represents copy and move assignments. And finally, `[EOL]` means that destructor of the object is called. 

The tool tries to show variable names that it knows about. However, each object has a unique number that allows to distinguish between temporary objects, as well as objects with the same name due to nested function calls. The unique number is displayed along with the name of the object and its address.

Also let's take a look to the graphical variant:

![](images/overview.png)

Rectangle node represents object, circle node is the operator and ellipse nodes represent the end of object's life. Nodes are ordered by program's execution flow. Copying, moving and objects relationship are represented with different edges.

Now let's use the logs to understand how the objects behave in the program. First, the "a" variable is created. Then, temporary objects corresponding to the function call arguments are created to be passed to the function. In the function, the passed objects are used to evaluate the expression, during which a temporary object 4 is created and destroyed after evaluation. After that, the result is moved to the caller and all temporary objects are destroyed. At the end, "c" is created as the copy of "b". 

In the next article, we will try to understand why the behavior is exactly like this.
