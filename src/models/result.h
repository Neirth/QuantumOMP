#include <sys/time.h>

typedef struct Result {
    struct timeval *start;
    struct timeval *end;
    double elapsed;
} Result;
