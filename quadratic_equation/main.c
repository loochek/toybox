#include <stdio.h>
#include <math.h>

// Помним, что даблы нельзя сравнивать с помощью ==, напишем свою функцию, сравнивая
// числа с погрешностью EPS

#define EPS 1e-7

int is_equal(double a, double b)
{
    return fabs(a - b) < EPS;
}

int main()
{
    double a, b, c;
    printf("Quadratic equation solver\nax^2 + bx + c = 0\nEnter a, b, c: ");
    scanf("%lf %lf %lf", &a, &b, &c);
    if (is_equal(a, 0))
    {
        printf("There is a linear equation! I can't solve this!\n");
        return 0;
    }
    double discr = b * b - 4 * a * c;
    if (is_equal(discr, 0))
    {
        printf("This equation has only one solution: x = %.4lf\n", -b / (2 * a));
    }
    else if (discr > 0)
        printf("This equation has two solutions: x = %.4lf or x = %.4lf\n", 
                                            (-b + sqrt(discr)) / (2 * a),
                                            (-b - sqrt(discr)) / (2 * a));
        
    else
        printf("This equation don't have any solutions\n");
    return 0;
}