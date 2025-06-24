#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/impl.h"

// Функции не из постановки задачи (для тестирования roots и integral)
double sample_f1(double x) {
    return (x - 2) * (x + 2);
}

double sample_f2(double x) {
    return -x * x + 3;
}

double sample_f3(double x) {
    return -(x - 1) * (x - 2);
}

bool check(double expected, double actual, int test_no, double eps) {
    if (fabs(expected - actual) > eps) {
        printf("Test %d failed:\n"
               "\tExpected: %.8f\n"
               "\tActual: %.8f\n"
               "\tDiff: %.8f\n", test_no, expected, actual,
               fabs(expected - actual));
        return false;
    }

    printf("Test %d: ok\n", test_no);

    return true;
}

bool test_root_against(double eps, double expected, double a, double b, double (*f_2)(double), double (*f_3)(double),
                       int test_no, int* total_iters) {
    root_results results = root(f_2, f_3, a, b, eps);

    double actual = results.root;

    if (total_iters != NULL) {
        *total_iters = results.iterations;
    }

    return check(expected, actual, test_no, eps);
}

void test_roots() {
    printf("===== Test roots =====\n");
    int failed_tests = 0;

    double eps = 1e-6;

    bool res1 = test_root_against(eps, 1.666666, 1.66, 1.67, sample_f2, sample_f3, 1, NULL);

    if (!res1)
        failed_tests++;

    bool res2 = test_root_against(eps, 2, 1.9, 2.1, sample_f1, sample_f3, 2, NULL);

    if (!res2)
        failed_tests++;

    bool res3 = test_root_against(eps, -1.870829, -1.88, -1.87, sample_f1, sample_f2, 3, NULL);

    if (!res3)
        failed_tests++;

    if (failed_tests == 0)
        printf("Test roots: ok\n");
    else
        printf("Test roots: failed %d tests\n", failed_tests);
}

void test_integral() {
    printf("===== Test integrals =====\n");
    int failed_tests = 0;

    double eps = 1e-5;
    double expected = -10.666667;
    double actual = integral(sample_f1, -2, 2);

    if (!check(expected, actual, 1, eps))
        failed_tests++;

    expected = 5.333333;
    actual = integral(sample_f2, -1, 1);

    if (!check(expected, actual, 2, eps))
        failed_tests++;

    expected = 0.166667;
    actual = integral(sample_f3, 1, 2);

    if (!check(expected, actual, 3, eps))
        failed_tests++;

    if (failed_tests == 0)
        printf("Test integral: ok\n");
    else
        printf("Test integral: failed %d tests\n", failed_tests);
}

void run_tests() {
    test_integral();
    test_roots();
}

int main() {
    run_tests();
}
