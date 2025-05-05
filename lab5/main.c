#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <array_size> <num_threads>\n", argv[0]);
        return 1;
    }

    int array_size = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    if (array_size <= 0 || num_threads <= 0) {
        printf("Array size and number of threads must be positive integers.\n");
        return 1;
    }

    // Генерация случайного массива
    int *array = (int *)malloc(array_size * sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < array_size; i++) {
        array[i] = rand() % 1000; // числа от 0 до 999
    }

    // printf("Original Array:\n");
    // for (int i = 0; i < array_size; i++) {
    //     printf("%d ", array[i]);
    // }
    // printf("\n");

    // Разбиение массива на части
    int chunk_size = (array_size + num_threads - 1) / num_threads;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    struct timeval start_time, end_time;

    // Запуск сортировки в потоках
    gettimeofday(&start_time, NULL);
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].array = array;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i + 1) * chunk_size;
        if (thread_data[i].end > array_size) {
            thread_data[i].end = array_size;
        }
        pthread_create(&threads[i], NULL, sort_thread, &thread_data[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    gettimeofday(&end_time, NULL);
    double sort_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                       (end_time.tv_usec - start_time.tv_usec) / 1000.0;

    // Слияние частей
    gettimeofday(&start_time, NULL);
    for (int size = chunk_size; size < array_size; size *= 2) {
        for (int i = 0; i < array_size; i += 2 * size) {
            int start1 = i;
            int end1 = i + size;
            int start2 = end1;
            int end2 = start2 + size;
            if (end1 > array_size) end1 = array_size;
            if (end2 > array_size) end2 = array_size;

            merge(array, start1, end1, start2, end2);
        }
    }
    gettimeofday(&end_time, NULL);
    double merge_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                        (end_time.tv_usec - start_time.tv_usec) / 1000.0;

    // printf("Sorted Array:\n");
    // for (int i = 0; i < array_size; i++) {
    //     printf("%d ", array[i]);
    // }
    // printf("\n");

    printf("Time taken for sorting: %.2f ms\n", sort_time);
    printf("Time taken for merging: %.2f ms\n", merge_time);

    free(array);
    return 0;
}