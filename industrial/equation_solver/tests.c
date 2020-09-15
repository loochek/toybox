#include <stdio.h>
#include "solver.h"

const int TEST_COUNT = 6;

const double tests[TEST_COUNT][3] = {
    { 0, 0, 0 },
    { 3, -14, -5 },
    { 9, -6, 1 },
    { 4, 0, 8 },
    { 0, -4, 8 },
    { 0, 0, 1 }
};

const equation_solution_t answers[TEST_COUNT] = {
    { INF_SOLUTIONS, 0, 0},
    { TWO_SOLUTIONS, 5, -1.0 / 3},
    { ONE_SOLUTION, 1.0 / 3, 0 },
    { NO_SOLUTIONS, 0, 0 },
    { ONE_SOLUTION, 2, 0 },
    { NO_SOLUTIONS, 0, 0 }
};

#ifndef TEST
int dummy();
#else
int main()
#endif
{
    for (int i = 0; i < TEST_COUNT; i++)
    {
        equation_solution_t ans = solve_equation(tests[i][0], tests[i][1], tests[i][2]);
        if (!(ans.status == answers[i].status && is_equal(ans.x1, answers[i].x1) && is_equal(ans.x2, answers[i].x2)))
        {
            printf("Test %i failed!\n", i + 1);
            return 0;
        }
    }
    printf("All tests passed!\n");
    return 0;
}