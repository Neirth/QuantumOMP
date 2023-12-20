#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifdef _OPENMP
    #include <omp.h>
#endif

#include "../../utils/timeDiff.h"
#include "../../models/result.h"

int mandelbrot(double real, double imag, int max_iter) {
    int iter;
    double r = real;
    double i = imag;

    for (iter = 0; iter < max_iter; ++iter) {
        double r2 = r * r;
        double i2 = i * i;

        if (r2 + i2 > 4.0) {
            return iter;
        }

        i = 2 * r * i + imag;
        r = r2 - i2 + real;
    }

    return max_iter;
}

void mandelbrot_set(int width, int height, double xmin, double xmax, double ymin, double ymax, int max_iter, int *output) {
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double real = xmin + x * (xmax - xmin) / (width - 1);
            double imag = ymin + y * (ymax - ymin) / (height - 1);

            output[y * width + x] = mandelbrot(real, imag, max_iter);
        }
    }
}

Result runTest() {
    int width = 800;
    int height = 600;
    double xmin = -2.0;
    double xmax = 1.0;
    double ymin = -1.5;
    double ymax = 1.5;
    int max_iter = 1000;

    int *output = (int *)malloc(width * height * sizeof(int));

    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);

    mandelbrot_set(width, height, xmin, xmax, ymin, ymax, max_iter, output);

    gettimeofday(&end, NULL);

    free(output);

    return (Result) {
        .start = &start,
        .end = &end,
        .elapsed = time_diff(&start, &end)
    };
}
