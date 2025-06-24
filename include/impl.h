#ifndef IMPL_H
#define IMPL_H

typedef double (fn)(double);

#define NUM_PARTITIONS 10000
#define EPS 1e-9

typedef struct {
    double root;
    int iterations;
} root_results;

root_results root(fn f, fn g, double a, double b, double eps1);

double integral(fn f, double a, double b);

double total_integral(double root1, double root2, double root3, double eps);

#endif //IMPL_H
