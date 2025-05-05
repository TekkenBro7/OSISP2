#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int *array;
    int start;
    int end;
} ThreadData;

void *sort_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int *arr = data->array;
    int start = data->start;
    int end = data->end;

    // Cортировка
    for (int i = start; i < end - 1; i++) {
        for (int j = start; j < end - 1 - (i - start); j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    pthread_exit(NULL);
}

void merge(int *array, int start1, int end1, int start2, int end2) {
    int size1 = end1 - start1;
    int size2 = end2 - start2;
    int *temp = (int *)malloc((size1 + size2) * sizeof(int));

    int i = start1, j = start2, k = 0;
    while (i < end1 && j < end2) {
        if (array[i] < array[j]) {
            temp[k++] = array[i++];
        } else {
            temp[k++] = array[j++];
        }
    }

    while (i < end1) temp[k++] = array[i++];
    while (j < end2) temp[k++] = array[j++];

    for (i = 0; i < size1 + size2; i++) {
        array[start1 + i] = temp[i];
    }

    free(temp);
}