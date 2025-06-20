#include <math.h>
#include <stdio.h>
typedef double (fn)(double);

#define NUM_PARTITIONS 1000

signed char sign(double n) {
    if (n > 0) {
        return 1;
    }

    if (n == 0) {
        return 0;
    }

    return -1;
}

double root(fn f, fn g, double a, double b, double eps1) {
    while (fabs(a - b) > eps1) {
        double mid = (a + b) / 2;

        double f_a = f(a);
        double f_b = f(b);
        double f_mid = f(mid);

        double g_a = g(a);
        double g_b = g(b);
        double g_mid = g(mid);

        double sign_a = sign(f_a - g_a);
        double sign_b = sign(f_b - g_b);
        double sign_mid = sign(f_mid - g_mid);

        if (sign_a != sign_mid) {
            b = mid;
        }
        else {
            a = mid;
        }
    }

    return a;
}

double f_i(fn f, double a, int i, double h) { return f(a + i * h); }

double integral(fn f, double a, double b, double eps2) {
    // Количество разбиений отрезка
    int n = NUM_PARTITIONS;

    double h = (b - a) / n;

    double s = f_i(f, a, 0, h) + f_i(f, a, n, h);

    for (int i = 1; i < n - 1; ++i) {
        int coef = i % 2 == 0 ? 2 : 4;

        s += coef * f_i(f, a, i, h);
    }

    return h / 3 * s;
}

extern double f1(double x);
extern double f2(double x);
extern double f3(double x);

double partial_integral(fn f1, fn f2, double root1, double root2, double eps) {
    // Интегралы функций на участках
    double int_f1 = integral(f1, root1, root2, eps);
    double int_f2 = integral(f2, root1, root2, eps);

    // Общий интеграл на участке
    double result = int_f1 - int_f2;

    return result;
}

double total_integral(double root1, double root2, double root3, double eps) {
    double i1 = partial_integral(f1, f3, root1, root2, eps);
    double i2 = partial_integral(f1, f2, root2, root3, eps);

    return i1 + i2;
}

int main(void) {
    double eps1 = 1e-9;
    double eps2 = eps1;

    // Отрезок, на котором находится точка пересечения кривых 1 и 3
    double x1 = -1.31;
    double x2 = -1.3;

    // Абсцисса пересечения кривых 1 и 3
    double root1 = root(f1, f3, x1, x2, eps1);

    // Отрезок, на котором находится точка пересечения кривых 2 и 3
    x1 = 0.82;
    x2 = 0.83;

    // Абсцисса пересечения кривых 2 и 3
    double root2 = root(f2, f3, x1, x2, eps1);

    // Отрезок, на котором находится точка пересечения кривых 1 и 2
    x1 = 1.34;
    x2 = 1.35;

    // Абсцисса пересечения кривых 1 и 2
    double root3 = root(f1, f2, x1, x2, eps1);

    printf("%f", total_integral(root1, root2, root3, eps2));
}
