#include <math.h>

#include "solver.h"

static const double EPS = 1e-7;

int is_equal(double a, double b)
{
    return fabs(a - b) < EPS;
}

// решает линейные уравнения
static equation_solution_t solve_linear_equation(double b, double c)
{
    if (is_equal(b, 0))
    {
        if (is_equal(c, 0))
            return (equation_solution_t){ INF_SOLUTIONS, 0, 0};
        else
            return (equation_solution_t){ NO_SOLUTIONS, 0, 0};
    }
    else
        return (equation_solution_t){ONE_SOLUTION, -c / b, 0};
}

equation_solution_t solve_equation(double a, double b, double c)
{
    if (is_equal(a, 0))
        return solve_linear_equation(b, c);
    double discr = b * b - 4 * a * c;
    if (is_equal(discr, 0))
        return (equation_solution_t){ ONE_SOLUTION, -b / (2 * a), 0 };
    else if (discr > 0)
        return (equation_solution_t){ TWO_SOLUTIONS, (-b + sqrt(discr)) / (2 * a),
                                                     (-b - sqrt(discr)) / (2 * a) };
    else
        return (equation_solution_t){ NO_SOLUTIONS, 0, 0 };
    
}