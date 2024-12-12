#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <omp.h>

#include "delta.h"


void shellsort(int *array, int length, int threads) {
    int delta;
    for (int k = 0; k < delta_size; ++k){
        delta = deltas[k];
        #pragma omp parallel for shared(delta, length, array) default(none) num_threads(threads)
        for(int i = 0; i < delta; ++i){
            for(int part = i + delta; part < length; part += delta){
                for(int j = part; j > i && array[j - delta] > array[j]; j -= delta){
                    int tmp = array[j];
                    array[j] = array[j - delta];
                    array[j - delta] = tmp;
                }
            }
        }
    }
}


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

    shellsort(array, length, threads);

    double time_end = omp_get_wtime();

    assert(is_sorted(array, length));

    printf("%g\n", time_end - time_start);
    
    free(array);

    return 0;
}
