#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
    int result = max_iter;
    int terminate = false;

    #pragma omp parallel for default(none) shared(r, i, max_iter, real, imag, result, terminate) private(iter)
    for (iter = 0; iter < max_iter; ++iter) {
        if (!terminate) {
            double r2 = r * r;
            double i2 = i * i;

            if (r2 + i2 > 4.0) {
                {
                    // Actualizar el resultado solo si no ha sido actualizado por otro hilo
                    if (result == max_iter) {
                        result = iter;
                    }
                }
                // Salir del bucle una vez que la condición se ha cumplido
                terminate = true;
            }

            i = 2 * r * i + imag;
            r = r2 - i2 + real;
        } else {
            #ifndef _OPENMP
                break;
            #endif
        }
    }

    return result;
}

void mandelbrot_set(int width, int height, double xmin, double xmax, double ymin, double ymax, int max_iter, int *output) {
    #pragma omp parallel for collapse(2) default(none) shared(width, height, xmin, xmax, ymin, ymax, max_iter, output)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double real = xmin + x * (xmax - xmin) / (width - 1);
            double imag = ymin + y * (ymax - ymin) / (height - 1);

            output[y * width + x] = mandelbrot(real, imag, max_iter);
        }
    }
}

Result runTest(void) {
    int width = 3840;
    int height = 2160;
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
