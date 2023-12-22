#include <stdio.h>
#include <dlfcn.h>
#include <malloc.h>
#include "models/result.h"

#define TOTAL_ALGORITHMS 2
#define TOTAL_RUNTIMES 4

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

    Result result = runTest();

    printf("| %-10s | %-10s | %-10ld | %-10ld | %-10g |\n", algorithm, runtime, result.start->tv_sec, result.end->tv_sec, result.elapsed);
}

int main() {
    char *algorithm[TOTAL_ALGORITHMS] = {"mandelbrot", "dijkstra"};
    char *runtime[TOTAL_RUNTIMES] = {"nvidia", "intel", "unix", "sequential"};

    printf("------------------------------------------------------------------\n");
    printf("| %-10s | %-10s | %-10s | %-10s | %-10s |\n", "Algorithm", "Runtime", "Start", "End", "Elapsed");

    for (int i = 0; i < TOTAL_ALGORITHMS; i++) {
        printf("------------------------------------------------------------------\n");

        for (int j = 0; j < TOTAL_RUNTIMES; j++) {
            runAndTestAlgorithm(algorithm[i], runtime[j]);
        }
    }

    printf("------------------------------------------------------------------\n");

    return 0;
}
