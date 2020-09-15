#include <stdio.h>

#include "solver.h"

#ifdef TEST
int dummy();
#else
int main()
#endif
{
    double  a = 0, 
            b = 0,
            c = 0;
    printf("Equation solver\nax^2 + bx + c = 0\nEnter a, b, c: ");
    scanf("%lf %lf %lf", &a, &b, &c);
    equation_solution_t sol = solve_equation(a, b, c);
    if (sol.status == ONE_SOLUTION)
        printf("This equation has only one solution: x = %.4lf\n", sol.x1);
    else if (sol.status == TWO_SOLUTIONS)
        printf("This equation has two solutions: x = %.4lf or x = %.4lf\n", sol.x1, sol.x2);
    else if (sol.status == NO_SOLUTIONS)
        printf("This equation don't have any solutions\n");
    else
        printf("Any x is the solution of this equation\n");
    return 0;
}