//
// Created by neirth on 4/12/23.
//
#include <sys/time.h>

#ifndef BENCHOPENMP_TIMEDIFF_H
#define BENCHOPENMP_TIMEDIFF_H

double time_diff(struct timeval *start, struct timeval *end);
#endif //BENCHOPENMP_TIMEDIFF_H
