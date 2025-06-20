#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>

typedef double (fn)(double);

#define NUM_PARTITIONS 10000
#define EPS 1e-9

signed char sign(double n) {
    if (n > 0) {
        return 1;
    }

    if (n == 0) {
        return 0;
    }

    return -1;
}

typedef struct {
    double root;
    int iterations;
} root_results;

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

extern double f1(double x);
extern double f2(double x);
extern double f3(double x);

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

// Функции не из постановки задачи (для тестирования roots и integral
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

void find_roots(double* root1, double* root2, double* root3, int* total_iterations) {
    double x1 = -1.31;
    double x2 = -1.3;

    int total_iters = 0;

    root_results rres1 = root(f1, f3, x1, x2, EPS);
    *root1 = rres1.root;
    total_iters += rres1.iterations;

    // Отрезок, на котором находится точка пересечения кривых 2 и 3
    x1 = 0.82;
    x2 = 0.83;

    root_results rres2 = root(f2, f3, x1, x2, EPS);
    *root2 = rres2.root;
    total_iters += rres2.iterations;

    // Отрезок, на котором находится точка пересечения кривых 1 и 2
    x1 = 1.34;
    x2 = 1.35;

    root_results rres3 = root(f1, f2, x1, x2, EPS);
    *root3 = rres3.root;
    total_iters += rres3.iterations;

    if (total_iterations != NULL)
        *total_iterations = total_iters;
}

// Функция просто подсчитывает интеграл из условия и выводит его в stdout, ничего больше.
void display_answer() {
    // Отрезок, на котором находится точка пересечения кривых 1 и 3
    double root1;
    double root2;
    double root3;
    find_roots(&root1, &root2, &root3, NULL);

    printf("%f", total_integral(root1, root2, root3, EPS));
}

void run_tests() {
    test_integral();
    test_roots();
}

void display_verdict(double eps, double expected, double abs_error) {
    if (abs_error <= eps)
        printf("\tТест пройден\n");
    else
        printf("\tТест провален\n");

    printf("\n");
    printf("\tАбсолютная ошибка: %f\n", abs_error);
    printf("\tАбсолютная ошибка: %f\n", abs_error);
    printf("\tОтносительная ошибка: %f\n", abs_error / expected);
}

void handle_root_flag(int iterations_flag) {
    double r1, r2, r3;
    int total_iterations;

    find_roots(&r1, &r2, &r3, &total_iterations);

    printf("Абсциссы пересечений:\n"
           "\tx1 - %f\n"
           "\tx2 - %f\n"
           "\tx3 - %f\n",
           r1, r2, r3);

    if (iterations_flag)
        printf("\nОбщее число итераций - %d\n", total_iterations);

    printf("\n");
}

bool handle_test_root(int iterations_flag, char* root_test_options, int* value) {
    printf("=== Тестирование root ===\n");

    if (root_test_options == NULL) {
        printf("Ошибка: параметр не задан\n");
        *value = 1;
        return true;
    }

    int func_ind_1, func_ind_2;
    double a, b, eps, expected;

    sscanf(root_test_options, "%d:%d:%lf:%lf:%lf:%lf", &func_ind_1, &func_ind_2, &a, &b, &eps, &expected);

    fn* q;
    fn* w;

    switch (func_ind_1) {
    case 1:
        q = f1;
        break;

    case 2:
        q = f2;
        break;

    case 3:
    default:
        q = f3;
        break;
    }

    switch (func_ind_2) {
    case 1:
        w = f1;
        break;

    case 2:
        w = f2;
        break;

    case 3:
    default:
        w = f3;
        break;
    }

    int total_iters;
    // test_root_against(eps, correct, a, b, q, w, 0, &total_iters);
    root_results actual = root(q, w, a, b, eps);
    double abs_error = fabs(actual.root - expected);
    printf("Найденный корень: %f\n", actual.root);

    display_verdict(eps, expected, abs_error);

    if (iterations_flag)
        printf("\tКоличество итераций: %d\n", actual.iterations);
    return false;
}

bool handle_test_integral(char* integral_test_options, int* value) {
    printf("=== Тестирование integral ===\n");

    if (integral_test_options == NULL) {
        printf("Ошибка: параметр не задан\n");
        *value = 1;
        return true;
    }

    int func_ind;
    double a, b, eps, expected;

    sscanf(integral_test_options, "%d:%lf:%lf:%lf:%lf", &func_ind, &a, &b, &eps, &expected);

    fn* q;
    switch (func_ind) {
    case 1:
        q = f1;
        break;

    case 2:
        q = f2;
        break;

    case 3:
    default:
        q = f3;
        break;
    }

    double actual = integral(*q, a, b);

    double abs_error = fabs(expected - actual);
    display_verdict(eps, expected, abs_error);
    return false;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        display_answer();
        return 0;
    }

    int root_flag = 0;
    int iterations_flag = 0;
    int test_root_flag = 0;
    int test_integral_flag = 0;

    struct option options[] = {
        {"help", no_argument, 0, 1},
        {"root", no_argument, &root_flag, 1},
        {"iterations", no_argument, &iterations_flag, 1},
        {"test-root", required_argument, &test_root_flag, 1},
        {"test-integral", required_argument, &test_integral_flag, 1},
        {0, 0, 0, 0}
    };

    int opt;
    int _idx;
    char* integral_test_options = 0;
    char* root_test_options = 0;

    while ((opt = getopt_long(argc, argv, "hriR:I:", options, &_idx)) != -1) {
        int ret_code;

        switch (opt) {
        case 'h':
            printf("Справка\n"
                "\t-h, --help - вывести это сообщение\n"
                "\t-r, --root - напечатать абсциссы точек пересечения кривых\n"
                "\t-i, --iterations - напечатать число итераций, потребовавшихся на приближенное решение "
                "уравнений при поиске точек пересечения\n"
                "\t-R, --test-root F1:F2:A:B:E:R, где F1, F2 — номера используемых функций, A, B, E — значения "
                "параметров a, b, eps1 функции root, R - правильный ответ - протестировать функцию root\n"
                "\t-I, --test-integral F:A:B:E:R, где F — номера используемой функций, A, B, E — значения параметров "
                "a, b, eps2 функции integral, R — правильный ответ - протестировать функцию integral\n");
            break;

        case 'r':
            handle_root_flag(iterations_flag);
            break;
        case 'i':
            iterations_flag = 1;
            break;
        case 'R':
            root_test_options = optarg;
            // Функция вернет true, если root_test_options == null
            if (handle_test_root(iterations_flag, root_test_options, &ret_code)) return 1;
            break;
        case 'I':
            integral_test_options = optarg;
            if (handle_test_integral(integral_test_options, &ret_code)) return 1;
            break;

        case '?':
            printf("Неизвестная опция\n");
            break;

        default:
            break;
        }
    }

    if (root_flag) {
        handle_root_flag(iterations_flag);
    }

    if (test_root_flag) {
        int ret_code;
        // Функция вернет true, если root_test_options == null
        if (handle_test_root(iterations_flag, root_test_options, &ret_code)) return ret_code;
    }

    if (test_integral_flag) {
        int ret_code;
        // Аналогично
        if (handle_test_integral(integral_test_options, &ret_code)) return ret_code;
    }

    return 0;
}
