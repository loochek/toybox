# Move semantics - beginning

After we have learned how to work with the tool, I propose to analyze the lifetime of temporary objects and how move semantics allow us to write more efficient code.

There is a sample program. Note that there are no any explicit copying:

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

    INT(a, 1);
    INT(b, 2);
    INT(c, 3);

    INT(res2, 0);
    res2 = (a - b) * c;
}
```

Let's see how it worked in the pre-C++11 era without move semantics.

<details>
<summary>Tool output (no move semantics)</summary>
<pre>
void testEntry()
{
    [New] "temp object 0" (0|0x7fff738d56c0) <-- 10
    [New] "temp object 1" (1|0x7fff738d5678) <-- 20
    [New] "temp object 2" (2|0x7fff738d5630) <-- 30
    TrackedInt test(TrackedInt, TrackedInt, TrackedInt)
    {
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 3" (3|0x7fff738d5370) <--(30)-- "temp object 0" (0|0x7fff738d56c0) + "temp object 1" (1|0x7fff738d5678)
        }
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 4" (4|0x7fff738d56e8) <--(60)-- "temp object 3" (3|0x7fff738d5370) + "temp object 2" (2|0x7fff738d5630)
        }
        [EOL] "temp object 3" (3|0x7fff738d5370)
    }
    [<font color=#FF0000>COPY</font>] "res1" (5|0x7fff738d5710) <--(60)-- "temp object 4" (4|0x7fff738d56e8)
    [EOL] "temp object 4" (4|0x7fff738d56e8)
    [EOL] "temp object 2" (2|0x7fff738d5630)
    [EOL] "temp object 1" (1|0x7fff738d5678)
    [EOL] "temp object 0" (0|0x7fff738d56c0)
    [New] "a" (6|0x7fff738d55c8) <-- 1
    [New] "b" (7|0x7fff738d5580) <-- 2
    [New] "c" (8|0x7fff738d5538) <-- 3
    [New] "res2" (9|0x7fff738d54f0) <-- 0
    TrackedInt TrackedInt::operator-(const TrackedInt &) const
    {
        [New] "temp object 10" (10|0x7fff738d5480) <--(-1)-- "a" (6|0x7fff738d55c8) - "b" (7|0x7fff738d5580)
    }
    TrackedInt TrackedInt::operator*(const TrackedInt &) const
    {
        [New] "temp object 11" (11|0x7fff738d54a8) <--(-3)-- "temp object 10" (10|0x7fff738d5480) * "c" (8|0x7fff738d5538)
    }
    [<font color=#FF0000>Copy=</font>] "res2" (9|0x7fff738d54f0) <--(-3)-- "temp object 11" (11|0x7fff738d54a8)
    [EOL] "temp object 11" (11|0x7fff738d54a8)
    [EOL] "temp object 10" (10|0x7fff738d5480)
    [EOL] "res2" (9|0x7fff738d54f0)
    [EOL] "c" (8|0x7fff738d5538)
    [EOL] "b" (7|0x7fff738d5580)
    [EOL] "a" (6|0x7fff738d55c8)
    [EOL] "res1" (5|0x7fff738d5710)
}
Total: 7 tmp object, 2 copies
</pre>
</details>

Not so bad. But let think - we are passed temporary objects to the function, and they are used by `test` function directly. Shouldn't an additional copies in `test` have been made? Shouldn't a copy have been made? Should - but compiler optimization called "copy elision" took care about us. Let's disable it:

<details>
<summary>Tool output (no move semantics, copy elision disabled)</summary>
<pre>
void testEntry()
{
    [New] "temp object 0" (0|0x7ffe5d137798) <-- 10
    [<font color=#FF0000>COPY</font>] "temp object 1" (1|0x7ffe5d1377c0) <--(10)-- "temp object 0" (0|0x7ffe5d137798)
    [New] "temp object 2" (2|0x7ffe5d137708) <-- 20
    [<font color=#FF0000>COPY</font>] "temp object 3" (3|0x7ffe5d137730) <--(20)-- "temp object 2" (2|0x7ffe5d137708)
    [New] "temp object 4" (4|0x7ffe5d137678) <-- 30
    [<font color=#FF0000>COPY</font>] "temp object 5" (5|0x7ffe5d1376a0) <--(30)-- "temp object 4" (4|0x7ffe5d137678)
    TrackedInt test(TrackedInt, TrackedInt, TrackedInt)
    {
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 6" (6|0x7ffe5d137260) <--(30)-- "temp object 1" (1|0x7ffe5d1377c0) + "temp object 3" (3|0x7ffe5d137730)
            [<font color=#FF0000>COPY</font>] "temp object 7" (7|0x7ffe5d137348) <--(30)-- "temp object 6" (6|0x7ffe5d137260)
            [EOL] "temp object 6" (6|0x7ffe5d137260)
        }
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 8" (8|0x7ffe5d137260) <--(60)-- "temp object 7" (7|0x7ffe5d137348) + "temp object 5" (5|0x7ffe5d1376a0)
            [<font color=#FF0000>COPY</font>] "temp object 9" (9|0x7ffe5d137370) <--(60)-- "temp object 8" (8|0x7ffe5d137260)
            [EOL] "temp object 8" (8|0x7ffe5d137260)
        }
        [<font color=#FF0000>COPY</font>] "temp object 10" (10|0x7ffe5d1377e8) <--(60)-- "temp object 9" (9|0x7ffe5d137370)
        [EOL] "temp object 9" (9|0x7ffe5d137370)
        [EOL] "temp object 7" (7|0x7ffe5d137348)
    }
    [<font color=#FF0000>COPY</font>] "res1" (11|0x7ffe5d137810) <--(60)-- "temp object 10" (10|0x7ffe5d1377e8)
    [EOL] "temp object 10" (10|0x7ffe5d1377e8)
    [EOL] "temp object 5" (5|0x7ffe5d1376a0)
    [EOL] "temp object 4" (4|0x7ffe5d137678)
    [EOL] "temp object 3" (3|0x7ffe5d137730)
    [EOL] "temp object 2" (2|0x7ffe5d137708)
    [EOL] "temp object 1" (1|0x7ffe5d1377c0)
    [EOL] "temp object 0" (0|0x7ffe5d137798)
    [New] "a" (12|0x7ffe5d1375f0) <-- 1
    [New] "b" (13|0x7ffe5d1375a8) <-- 2
    [New] "c" (14|0x7ffe5d137560) <-- 3
    [New] "res2" (15|0x7ffe5d137518) <-- 0
    TrackedInt TrackedInt::operator-(const TrackedInt &) const
    {
        [New] "temp object 16" (16|0x7ffe5d137370) <--(-1)-- "a" (12|0x7ffe5d1375f0) - "b" (13|0x7ffe5d1375a8)
        [<font color=#FF0000>COPY</font>] "temp object 17" (17|0x7ffe5d1374a8) <--(-1)-- "temp object 16" (16|0x7ffe5d137370)
        [EOL] "temp object 16" (16|0x7ffe5d137370)
    }
    TrackedInt TrackedInt::operator*(const TrackedInt &) const
    {
        [New] "temp object 18" (18|0x7ffe5d137370) <--(-3)-- "temp object 17" (17|0x7ffe5d1374a8) * "c" (14|0x7ffe5d137560)
        [<font color=#FF0000>COPY</font>] "temp object 19" (19|0x7ffe5d1374d0) <--(-3)-- "temp object 18" (18|0x7ffe5d137370)
        [EOL] "temp object 18" (18|0x7ffe5d137370)
    }
    [<font color=#FF0000>Copy=</font>] "res2" (15|0x7ffe5d137518) <--(-3)-- "temp object 19" (19|0x7ffe5d1374d0)
    [EOL] "temp object 19" (19|0x7ffe5d1374d0)
    [EOL] "temp object 17" (17|0x7ffe5d1374a8)
    [EOL] "res2" (15|0x7ffe5d137518)
    [EOL] "c" (14|0x7ffe5d137560)
    [EOL] "b" (13|0x7ffe5d1375a8)
    [EOL] "a" (12|0x7ffe5d1375f0)
    [EOL] "res1" (11|0x7ffe5d137810)
}
Total: 15 tmp object, 10 copies
</pre>
</details>

Now the behavior is what we expect - passing temporary object to the function by value isn't optimized. Also, copy elision includes optimization called Return Value Optimization - note that compiler now creates additional object during statements like `return TrackedInt(..)`. In our program, such expressions appear in the `test` function and operators calls - `temp object 10` is the additional object during `test`'s return statement and, for example, `temp object 7` is the additional object during `operator+`'s return statement.

Let's return to the optimized variant. It would seem that optimizations solve all our problems. Not really. Let's take a look, for example, at the `res2` assignment - `temp object 11` is deleted immediately after it was copied. It is not a problem in our case because copying of TrackedInt is cheap, but copying of objects like std::string can be expensive. We could move it instead of copying. In fact, С++ provides us with a semantics by which we can mark objects with an expiring lifetime. Through the move constructor, we can define an action on a "dying" object - and "rob" it, instead of making an expensive copy, knowing that it no longer needs it's data.

Let's see how it works:

<details>
<summary>Tool output (move semantics, copy elision enabled)</summary>
<pre>
void testEntry()
{
    [New] "temp object 0" (0|0x7fff09e2c6e0) <-- 10
    [New] "temp object 1" (1|0x7fff09e2c698) <-- 20
    [New] "temp object 2" (2|0x7fff09e2c650) <-- 30
    TrackedInt test(TrackedInt, TrackedInt, TrackedInt)
    {
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 3" (3|0x7fff09e2c390) <--(30)-- "temp object 0" (0|0x7fff09e2c6e0) + "temp object 1" (1|0x7fff09e2c698)
        }
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 4" (4|0x7fff09e2c708) <--(60)-- "temp object 3" (3|0x7fff09e2c390) + "temp object 2" (2|0x7fff09e2c650)
        }
        [EOL] "temp object 3" (3|0x7fff09e2c390)
    }
    [<font color=#00FF00>Move</font>] "res1" (5|0x7fff09e2c730) <--(60)-- "temp object 4" (4|0x7fff09e2c708)
    [EOL] "temp object 4" (4|0x7fff09e2c708)
    [EOL] "temp object 2" (2|0x7fff09e2c650)
    [EOL] "temp object 1" (1|0x7fff09e2c698)
    [EOL] "temp object 0" (0|0x7fff09e2c6e0)
    [New] "a" (6|0x7fff09e2c5e8) <-- 1
    [New] "b" (7|0x7fff09e2c5a0) <-- 2
    [New] "c" (8|0x7fff09e2c558) <-- 3
    [New] "res2" (9|0x7fff09e2c510) <-- 0
    TrackedInt TrackedInt::operator-(const TrackedInt &) const
    {
        [New] "temp object 10" (10|0x7fff09e2c4a0) <--(-1)-- "a" (6|0x7fff09e2c5e8) - "b" (7|0x7fff09e2c5a0)
    }
    TrackedInt TrackedInt::operator*(const TrackedInt &) const
    {
        [New] "temp object 11" (11|0x7fff09e2c4c8) <--(-3)-- "temp object 10" (10|0x7fff09e2c4a0) * "c" (8|0x7fff09e2c558)
    }
    [<font color=#00FF00>Move=</font>] "res2" (9|0x7fff09e2c510) <--(-3)-- "temp object 11" (11|0x7fff09e2c4c8)
    [EOL] "temp object 11" (11|0x7fff09e2c4c8)
    [EOL] "temp object 10" (10|0x7fff09e2c4a0)
    [EOL] "res2" (9|0x7fff09e2c510)
    [EOL] "c" (8|0x7fff09e2c558)
    [EOL] "b" (7|0x7fff09e2c5a0)
    [EOL] "a" (6|0x7fff09e2c5e8)
    [EOL] "res1" (5|0x7fff09e2c730)
}
Total: 7 tmp object, 0 copies
</pre>
</details>

Zero copies! Since all copies are temporary objects, they are all moved.

With disabled copy elision, there are still a lot of temporary objects. However, they are all moved.

<details>
<summary>Tool output (move semantics, copy elision disabled)</summary>
<pre>
void testEntry()
{
    [New] "temp object 0" (0|0x7fff77d65cc8) <-- 10
    [<font color=#00FF00>Move</font>] "temp object 1" (1|0x7fff77d65cf0) <--(10)-- "temp object 0" (0|0x7fff77d65cc8)
    [New] "temp object 2" (2|0x7fff77d65c38) <-- 20
    [<font color=#00FF00>Move</font>] "temp object 3" (3|0x7fff77d65c60) <--(20)-- "temp object 2" (2|0x7fff77d65c38)
    [New] "temp object 4" (4|0x7fff77d65ba8) <-- 30
    [<font color=#00FF00>Move</font>] "temp object 5" (5|0x7fff77d65bd0) <--(30)-- "temp object 4" (4|0x7fff77d65ba8)
    TrackedInt test(TrackedInt, TrackedInt, TrackedInt)
    {
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 6" (6|0x7fff77d65790) <--(30)-- "temp object 1" (1|0x7fff77d65cf0) + "temp object 3" (3|0x7fff77d65c60)
            [<font color=#00FF00>Move</font>] "temp object 7" (7|0x7fff77d65878) <--(30)-- "temp object 6" (6|0x7fff77d65790)
            [EOL] "temp object 6" (6|0x7fff77d65790)
        }
        TrackedInt TrackedInt::operator+(const TrackedInt &) const
        {
            [New] "temp object 8" (8|0x7fff77d65790) <--(60)-- "temp object 7" (7|0x7fff77d65878) + "temp object 5" (5|0x7fff77d65bd0)
            [<font color=#00FF00>Move</font>] "temp object 9" (9|0x7fff77d658a0) <--(60)-- "temp object 8" (8|0x7fff77d65790)
            [EOL] "temp object 8" (8|0x7fff77d65790)
        }
        [<font color=#00FF00>Move</font>] "temp object 10" (10|0x7fff77d65d18) <--(60)-- "temp object 9" (9|0x7fff77d658a0)
        [EOL] "temp object 9" (9|0x7fff77d658a0)
        [EOL] "temp object 7" (7|0x7fff77d65878)
    }
    [<font color=#00FF00>Move</font>] "res1" (11|0x7fff77d65d40) <--(60)-- "temp object 10" (10|0x7fff77d65d18)
    [EOL] "temp object 10" (10|0x7fff77d65d18)
    [EOL] "temp object 5" (5|0x7fff77d65bd0)
    [EOL] "temp object 4" (4|0x7fff77d65ba8)
    [EOL] "temp object 3" (3|0x7fff77d65c60)
    [EOL] "temp object 2" (2|0x7fff77d65c38)
    [EOL] "temp object 1" (1|0x7fff77d65cf0)
    [EOL] "temp object 0" (0|0x7fff77d65cc8)
    [New] "a" (12|0x7fff77d65b20) <-- 1
    [New] "b" (13|0x7fff77d65ad8) <-- 2
    [New] "c" (14|0x7fff77d65a90) <-- 3
    [New] "res2" (15|0x7fff77d65a48) <-- 0
    TrackedInt TrackedInt::operator-(const TrackedInt &) const
    {
        [New] "temp object 16" (16|0x7fff77d658a0) <--(-1)-- "a" (12|0x7fff77d65b20) - "b" (13|0x7fff77d65ad8)
        [<font color=#00FF00>Move</font>] "temp object 17" (17|0x7fff77d659d8) <--(-1)-- "temp object 16" (16|0x7fff77d658a0)
        [EOL] "temp object 16" (16|0x7fff77d658a0)
    }
    TrackedInt TrackedInt::operator*(const TrackedInt &) const
    {
        [New] "temp object 18" (18|0x7fff77d658a0) <--(-3)-- "temp object 17" (17|0x7fff77d659d8) * "c" (14|0x7fff77d65a90)
        [<font color=#00FF00>Move</font>] "temp object 19" (19|0x7fff77d65a00) <--(-3)-- "temp object 18" (18|0x7fff77d658a0)
        [EOL] "temp object 18" (18|0x7fff77d658a0)
    }
    [<font color=#00FF00>Move=</font>] "res2" (15|0x7fff77d65a48) <--(-3)-- "temp object 19" (19|0x7fff77d65a00)
    [EOL] "temp object 19" (19|0x7fff77d65a00)
    [EOL] "temp object 17" (17|0x7fff77d659d8)
    [EOL] "res2" (15|0x7fff77d65a48)
    [EOL] "c" (14|0x7fff77d65a90)
    [EOL] "b" (13|0x7fff77d65ad8)
    [EOL] "a" (12|0x7fff77d65b20)
    [EOL] "res1" (11|0x7fff77d65d40)
}
Total: 15 tmp object, 0 copies
</pre>
</details>

## Conslusion

Due to the tool, we managed to optimize unnecessary copying of temporary objects. We also discovered behaviour of the compilier with disabled "copy elision" optimizations. Let's summarize the results:

|                       | Without move             | With move               |
| --------------------- | ------------------------ | ----------------------- |
| Copy elision disabled | 🔴15 tmp objs, 10 copies | 🟡15 tmp objs, 0 copies |
| Copy elision enabled  | 🟠7 tmp objs, 2 copies   | 🟢7 tmp objs, 0 copies  |

In the next article, we will look at less trivial use cases of move semantics.
