#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <assert.h>

#include "delta.h"


bool is_sorted(int *array, int length) {
    bool answer = true;
    for (int i = 1; i < length; ++i) {
        if (array[i - 1] > array[i]) {
            printf("wrong %d, %d at %d, %d\n", array[i - 1], array[i], i - 1, i);
            answer = false;
        }
    }
    return answer;
}


void shellsort(int *array, int length) {
    int delta;
    for (int k = 0; k < delta_size; ++k) {
        delta = deltas[k];
        for (int i = delta; i < length; ++i) {
            for (int j = i - delta; j >= 0 && array[j] > array[j + delta]; j -= delta) {
                int temp = array[j];
                array[j] = array[j + delta];
                array[j + delta] = temp;
            }
        }
    }
}


void merge(int *arr, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = 0;
    int *temp = (int *)malloc((right - left + 1) * sizeof(int));

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (i = left, k = 0; i <= right; i++, k++) {
        arr[i] = temp[k];
    }

    free(temp);
}


int *generate_random_array(unsigned int random_seed, unsigned int length) {
    int *array = (int*)malloc(length * sizeof(int));
    srand(random_seed);
    for(int i=0; i < length; i++) { array[i] = rand(); }
    return array;
}


int main(int argc, char **argv){
    const unsigned int length = 1 << 22;
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int random_seed = strtoul(argv[2], NULL, 10);

    unsigned int part_size = length / threads;

    int *array = generate_random_array(random_seed, length);

    double time_start = omp_get_wtime();

    #pragma omp parallel num_threads(threads) default(none) shared(array, threads, part_size, length)
    {
        int i = omp_get_thread_num();
        int prev_size = i * part_size;

        shellsort(array + prev_size, (i == threads - 1) ? (length - prev_size) : part_size);

        #pragma omp barrier

        while (part_size < length) {
            int j = i * (part_size << 1);
            int mid = j + part_size - 1;
            int right = (j + 2 * part_size - 1 < length) ? (j + 2 * part_size - 1) : (length - 1);
            if (mid < right) {
                merge(array, j, mid, right);
            }
            
            #pragma omp barrier
            if (i == 0)
                part_size *= 2;
            #pragma omp barrier
        }
    }

    double time_end = omp_get_wtime();

    assert(is_sorted(array, length));

    printf("%g\n", time_end - time_start);

    free(array);

    return 0;
}
