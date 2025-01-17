#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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


void merge_blocks(int *start, unsigned int block_size, unsigned int n) {
    unsigned int total_elements = block_size * n;
    unsigned int current_block_size = block_size;

    while (current_block_size < total_elements) {
        for (unsigned int i = 0; i < total_elements; i += 2 * current_block_size) {
            unsigned int left = i;
            unsigned int mid = i + current_block_size - 1;
            unsigned int right = i + 2 * current_block_size - 1;

            if (mid >= total_elements) {
                break;
            }

            if (right >= total_elements) {
                right = total_elements - 1;
            }

            merge(start, left, mid, right);
        }
        current_block_size *= 2;
    }
}


int *generate_random_array(unsigned int random_seed, unsigned int length) {
    int *array = (int*)malloc(length * sizeof(int));
    srand(random_seed);
    for(int i=0; i < length; i++) { array[i] = rand(); }
    return array;
}


int main(int argc, char **argv){
    const unsigned int length = (1 << 22) + 19136;
    unsigned int chunks = 27720;
    const unsigned int random_seed = strtoul(argv[1], NULL, 10);

    int rank, size;
    int *displacements, *send_counts, *array;
    int *local_array;
    double time_start, time_end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const unsigned int block_size = length / size;
    const unsigned int chunk_size = length / chunks;
    const unsigned int chunks_in_block = chunks / size;


    int *sub_array = (int *)malloc(block_size * sizeof(int));
    if (!rank) {
        array = generate_random_array(random_seed, length);
        displacements = (int *)malloc(size * sizeof(int));
        send_counts = (int *)malloc(size * sizeof(int));

        for (int i = 0; i < size; ++i) {
            send_counts[i] = block_size;
            displacements[i] = i * block_size;
        }
    }

    time_start = MPI_Wtime();

    MPI_Scatterv(array, send_counts, displacements, MPI_INT, sub_array, block_size, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunks_in_block; ++i) {
        shellsort(sub_array + chunk_size * i, chunk_size);
    }

    merge_blocks(sub_array, chunk_size, chunks_in_block);

    MPI_Gatherv(sub_array, block_size, MPI_INT, array, send_counts, displacements, MPI_INT, 0, MPI_COMM_WORLD);

    if (!rank) {
        merge_blocks(array, block_size, size);

        time_end = MPI_Wtime();

        assert(is_sorted(array, length));

        printf("%g\n", time_end - time_start);
        free(array);
        free(displacements);
        free(send_counts);
    }

    free(sub_array);
    MPI_Finalize();
    return 0;
}
