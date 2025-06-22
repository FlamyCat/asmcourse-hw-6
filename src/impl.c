//
// Created by Aleksandr on 20.06.25.
//

#include "impl.h"

#include <math.h>

signed char sign(double n) {
    if (n > 0) {
        return 1;
    }

    if (n == 0) {
        return 0;
    }

    return -1;
}

root_results root(fn f, fn g, double a, double b, double eps1) {
    int iterations = 0;
    while (1) {
        double mid = (a + b) / 2;

        double f_a = f(a);
        double f_mid = f(mid);

        double g_a = g(a);
        double g_mid = g(mid);

        if (fabs(f_mid - g_mid) < eps1 && fabs(f_a - g_a) < eps1) {
            root_results r;
            r.root = mid;
            r.iterations = iterations;
            return r;
        }

        double sign_a = sign(f_a - g_a);
        double sign_mid = sign(f_mid - g_mid);

        if (sign_a != sign_mid) {
            iterations++;
            b = mid;
        }
        else {
            iterations++;
            a = mid;
        }
    }
}

double f_i(fn f, double a, int i, double h) {
    return f(a + i * h);
}

double integral(fn f, double a, double b) {
    int n = NUM_PARTITIONS;
    double h = (b - a) / n;
    double s = f_i(f, a, 0, h) + f_i(f, a, n, h);

    for (int i = 1; i < n; i++) {
        int coefficient = i % 2 == 1 ? 4 : 2;
        s += coefficient * f_i(f, a, i, h);
    }

    return h / 3 * s;
}

double partial_integral(fn f1, fn f2, double root1, double root2, double eps) {
    // Интегралы функций на участках
    double int_f1 = integral(*f1, root1, root2);
    double int_f2 = integral(*f2, root1, root2);

    // Общий интеграл на участке
    double result = int_f1 - int_f2;

    return result;
}

double total_integral(double root1, double root2, double root3, double eps) {
    double i1 = partial_integral(f1, f3, root1, root2, eps);
    double i2 = partial_integral(f1, f2, root2, root3, eps);

    return i1 + i2;
}
