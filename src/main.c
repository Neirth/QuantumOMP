#include <stdio.h>
#include <dlfcn.h>
#include <malloc.h>
#include "models/result.h"

Result (*runTest)(void);

char *formatFilename(char *algorithm, char *runtime) {
    char *filename = malloc(sizeof(char) * 100);
    sprintf(filename, "./lib%s_%s.so", algorithm, runtime);
    return filename;
}

void runAndTestAlgorithm(char *algorithm, char *runtime) {
    void* openResult = dlopen(formatFilename(algorithm, runtime), RTLD_LAZY);

    if (!openResult) {
        printf("Error: %s\n", dlerror());
        return;
    }

    *(void**)(&runTest) = dlsym(openResult, "runTest");

    Result result = (Result) runTest();

    printf("| %-10s | %-10s | %-10d | %-10d | %-10g |\n", algorithm, runtime, result.start, result.end, result.elapsed);

    int closeResult = dlclose(openResult);

    if (closeResult != 0) {
        printf("Error: %s\n", dlerror());
    }
}

int main() {
    char *algorithm[2] = {"mandelbrot", "dijkstra"};
    char *runtime[4] = {"sequential", "intel", "nvidia", "unix"};

    printf("------------------------------------------------------------------\n");
    printf("| %-10s | %-10s | %-10s | %-10s | %-10s |\n", "Algorithm", "Runtime", "Start", "End", "Elapsed");

    for (int i = 0; i < 2; i++) {
        printf("------------------------------------------------------------------\n");

        for (int j = 0; j < 4; j++) {
            runAndTestAlgorithm(algorithm[i], runtime[j]);
        }
    }

    printf("------------------------------------------------------------------\n");

    return 0;
}
