#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <getopt.h>

#include "impl.h"

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

    printf("%f\n", total_integral(root1, root2, root3, EPS));
}

void display_answer_with_iterations() {
    double root1;
    double root2;
    double root3;
    int iterations;
    find_roots(&root1, &root2, &root3, &iterations);

    printf("%f %d", total_integral(root1, root2, root3, EPS), iterations);
}

void display_verdict(double actual, double expected, double abs_error) {
    printf("%lf %lf %lf\n", actual, abs_error, abs_error / expected);
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

bool handle_test_root(int iterations_flag, const char* root_test_options, int* value) {
    if (root_test_options == NULL) {
        printf("Ошибка: параметр не задан\n");
        *value = 1;
        return true;
    }

    int func_ind_1, func_ind_2;
    double a, b, eps, expected;

    sscanf(root_test_options, "%d:%d:%lf:%lf:%lf:%lf", &func_ind_1, &func_ind_2, &a, &b, &eps, &expected); // NOLINT(*-err34-c)

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

    root_results actual = root(q, w, a, b, eps);
    double abs_error = fabs(actual.root - expected);
    display_verdict(actual.root, expected, abs_error);

    if (iterations_flag)
        printf("Количество итераций: %d\n", actual.iterations);
    return false;
}

bool handle_test_integral(const char* integral_test_options, int* value) {
    if (integral_test_options == NULL) {
        printf("Ошибка: параметр не задан\n");
        *value = 1;
        return true;
    }

    int func_ind;
    double a, b, eps, expected;

    sscanf(integral_test_options, "%d:%lf:%lf:%lf:%lf", &func_ind, &a, &b, &eps, &expected); // NOLINT(*-err34-c)

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
    display_verdict(actual, expected, abs_error);
    return false;
}

void print_help() {
    printf("Справка\n"
        "\t-h, --help - вывести это сообщение\n"
        "\t-r, --root - напечатать абсциссы точек пересечения кривых\n"
        "\t-i, --iterations - напечатать число итераций, потребовавшихся на приближенное решение "
        "уравнений при поиске точек пересечения\n"
        "\t-R, --test-root F1:F2:A:B:E:R, где F1, F2 — номера используемых функций, A, B, E — значения "
        "параметров a, b, eps1 функции root, R - правильный ответ - протестировать функцию root\n"
        "\t-I, --test-integral F:A:B:E:R, где F — номера используемой функций, A, B, E — значения параметров "
        "a, b, eps2 функции integral, R — правильный ответ - протестировать функцию integral\n");
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
    int help_flag = 0;

    struct option options[] = {
        {"help", no_argument, &help_flag, 1},
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
            print_help();
            break;

        case 'r':
            handle_root_flag(iterations_flag);
            break;
        case 'i':
            iterations_flag = 1;
            display_answer_with_iterations();
            return 0;
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

    if (iterations_flag) {
        display_answer_with_iterations();
        return 0;
    }

    if (help_flag) {
        print_help();
        return 0;
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
