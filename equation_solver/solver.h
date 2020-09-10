typedef struct
{
    enum status_t
    {
        NO_SOLUTIONS,
        ONE_SOLUTION,
        TWO_SOLUTIONS,
        INF_SOLUTIONS,
    } status;
    double x1;
    double x2;
} equation_solution_t;

// Помним, что даблы нельзя сравнивать с помощью ==, напишем свою функцию, сравнивая
// числа с погрешностью EPS
int is_equal(double a, double b);

// решает квадратные и линейные уравнения
equation_solution_t solve_equation(double a, double b, double c);