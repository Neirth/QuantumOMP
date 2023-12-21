#ifdef _OPENMP
    #include <omp.h>
#endif

#include <stdlib.h>
#include <float.h>
#include <sys/time.h>

#include "../../models/result.h"
#include "../../utils/timeDiff.h"

#define MATRIX_SIZE 4096

void initialize_algorithm_buffers(int gid, float *result, float *distance, int *visited, float *vertex, float *vertex_temp) {
    if (gid == 0) {
        visited[gid] = 1;
        result[gid] = 0;
    } else {
        visited[gid] = 0;
        result[gid] = FLT_MAX;
    }

    distance[gid] = 0;
    vertex[gid] = 0;
    vertex_temp[gid] = 0;
}

void shortest_path_algorithm(int gid, const float *result, const float *matrix_row, float *distance, int *visited, float *vertex_temp, int vertex_count) {
    if (visited[gid] != 1) {
        visited[gid] = 1;

        #pragma omp parallel for
        for (int edge = 0; edge < vertex_count; edge++) {
            float weight = matrix_row[edge];

            if (weight != 0.0f && weight != FLT_MAX) {
                float dist = result[edge] + weight;

                if (distance[gid] == 0.0 || result[gid] > dist) {
                    distance[gid] = dist;
                    vertex_temp[gid] = (float) edge;
                }
            }
        }
    }
}

void merge_sortest_path(int gid, float *result, const float *distance, int *visited, float *vertex, const float *vertex_temp) {
    if (result[gid] > distance[gid]) {
        result[gid] = distance[gid];
        vertex[gid] = vertex_temp[gid];
    }

    if (gid != 0) {
        visited[gid] = 0;
    }
}

float** generateMatrix(void) {
    float** matrix = (float**)malloc(MATRIX_SIZE * sizeof(float*));

    #pragma omp parallel default(none) shared(matrix)
    {
        #pragma omp for
        for (int i = 0; i < MATRIX_SIZE; i++) {
            matrix[i] = (float*)malloc(MATRIX_SIZE * sizeof(float));
        }

        #pragma omp for collapse(2)
        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                matrix[i][j] = ((float)rand() / RAND_MAX);
            }
        }
    }

    return matrix;
}


Result runTest(void) {
    struct timeval start;
    struct timeval end;

    // Matrix size
    int matrix_size = MATRIX_SIZE;

    // Matrix representation (replace with your actual data)
    float** matrix = generateMatrix();

    // Result, distance, vertex, vertex_temp, visited arrays
    float result[MATRIX_SIZE];
    float distance[MATRIX_SIZE];
    float vertex[MATRIX_SIZE];
    float vertex_temp[MATRIX_SIZE];
    int visited[MATRIX_SIZE];

    gettimeofday(&start, NULL);


    #pragma omp parallel for default(none) shared(result, distance, visited, vertex, vertex_temp)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        initialize_algorithm_buffers(i, result, distance, visited, vertex, vertex_temp);
    }

    #pragma omp parallel for default(none) shared(result, distance, visited, vertex, vertex_temp, matrix, matrix_size)
    for (int i = 0; i < MATRIX_SIZE; i++) {
        shortest_path_algorithm(i, result, matrix[i], distance, visited, vertex_temp, MATRIX_SIZE);
    }

    #pragma omp parallel for default(none) shared(result, distance, visited, vertex, vertex_temp, matrix, matrix_size)
    for (int i = 0; i < matrix_size; i++) {
        merge_sortest_path(i, result, distance, visited, vertex, vertex_temp);
    }

    gettimeofday(&end, NULL);

    for (int i = 0; i < MATRIX_SIZE; i++) {
        free(matrix[i]);
    }

    free(matrix);

    return (Result) {
        .start = &start,
        .end = &end,
        .elapsed = time_diff(&start, &end)
    };
}
