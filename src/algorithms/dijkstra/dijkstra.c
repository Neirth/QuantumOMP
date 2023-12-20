#ifdef _OPENMP
    #include <omp.h>
#endif

#include <stdio.h>
#include <float.h>
#include <sys/time.h>

#include "../../models/result.h"
#include "../../utils/timeDiff.h"

int global_gid = 0;

void initialize_algorithm_buffers(float *result, float *distance, int *visited, float *vertex, float *vertex_temp, int vertex_count) {
    int gid;

    #ifdef _OPENMP
        gid = omp_get_thread_num();
    #else
        gid = global_gid;
    #endif

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

void shortest_path_algorithm(float *result, float *matrix_row, float *distance, int *visited, float *vertex_temp, int vertex_count) {
    int gid;

    #ifdef _OPENMP
        gid = omp_get_thread_num();
    #else
        gid = global_gid;
    #endif

    if (visited[gid] != 1) {
        visited[gid] = 1;

        for (int edge = 0; edge < vertex_count; edge++) {
            float weight = matrix_row[edge];

            if (weight != 0.0f && weight != FLT_MAX) {
                float dist = result[edge] + weight;

                if (distance[gid] == 0.0 || result[gid] > dist) {
                    distance[gid] = dist;
                    vertex_temp[gid] = edge;
                }
            }
        }
    }
}

void merge_sortest_path(float *result, float *distance, int *visited, float *vertex, float *vertex_temp, int vertex_count) {
    int gid;

    #ifdef _OPENMP
        gid = omp_get_thread_num();
    #else
        gid = global_gid;
    #endif

    if (result[gid] > distance[gid]) {
        result[gid] = distance[gid];
        vertex[gid] = vertex_temp[gid];
    }

    if (gid != 0) {
        visited[gid] = 0;
    }
}

Result runTest() {
    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);

    // Matrix size
    int matrix_size = 6;

    // Matrix representation (replace with your actual data)
    float matrix[6][6] = {
            {01.0, 04.0, 02.0, 00.0, 00.0, 00.0},
            {04.0, 01.0, 01.0, 05.0, 00.0, 00.0},
            {02.0, 01.0, 01.0, 08.0, 10.0, 00.0},
            {00.0, 05.0, 08.0, 01.0, 02.0, 06.0},
            {00.0, 00.0, 10.0, 02.0, 01.0, 02.0},
            {00.0, 00.0, 00.0, 06.0, 02.0, 01.0}
    };

    // Result, distance, vertex, vertex_temp, visited arrays
    float result[matrix_size];
    float distance[matrix_size];
    float vertex[matrix_size];
    float vertex_temp[matrix_size];
    int visited[matrix_size];

    // Parallel initialization of buffers
    #pragma omp parallel for
    for (int i = 0; i < matrix_size; i++) {
        #ifndef _OPENMP
            global_gid = i;
        #endif

        #pragma omp task
        initialize_algorithm_buffers(result, distance, visited, vertex, vertex_temp, matrix_size);
    }

    // Parallel shortest path algorithm
    #pragma omp parallel for
    for (int i = 0; i < matrix_size; i++) {
        #ifndef _OPENMP
            global_gid = i;
        #endif

        #pragma omp task
        shortest_path_algorithm(result, matrix[i], distance, visited, vertex_temp, matrix_size);
    }

    // Parallel merge the shortest path
    #pragma omp parallel for
    for (int i = 0; i < matrix_size; i++) {
        #ifndef _OPENMP
            global_gid = i;
        #endif

        #pragma omp task
        merge_sortest_path(result, distance, visited, vertex, vertex_temp, matrix_size);
    }

    gettimeofday(&end, NULL);

    return (Result) {
        .start = &start,
        .end = &end,
        .elapsed = time_diff(&start, &end)
    };
}
