#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    long int n = 100000;
    double e = 1E-10;
    double *t = malloc(sizeof(double) * n);

    for (long int i = 0; i < n; i++) {
        t[i] = (double) i / e;
    }

    double s1 = 0;
    for (long int i = 0; i < n; i++) {
        s1 += t[i];
    }

    double sum[2] = {0, 0};
    for (long int i = 0; i < n / 2; i++) {
        sum[0] = sum[0] + t[i * 2 + 0];
        sum[1] = sum[1] + t[i * 2 + 1];
    }
    double s2 = sum[0] + sum[1];

    printf("%.15g\n", s1);
    printf("%.15g\n", s2);

    return 0;
}

