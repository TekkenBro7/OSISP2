#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>

typedef struct {
    int *array;
    int start;
    int end;
} ThreadData;

void *sort_thread(void *arg);
void merge(int *array, int start1, int end1, int start2, int end2);

#endif // UTILS_H