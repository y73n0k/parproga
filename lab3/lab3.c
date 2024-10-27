#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>


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
    for (int delta = length / 2; delta > 0; delta /= 2) {
        for (int i = delta; i < length; ++i) {
            for (int j = i - delta; j >= 0 && array[j] > array[j + delta]; j -= delta) {
                int temp = array[j];
                array[j] = array[j + delta];
                array[j + delta] = temp;
            }
        }
    }
}


int *merge(int *array1, int *array2, int n1, int n2) {
    int *result = (int*)malloc((n1 + n2) * sizeof(int));
    int i = 0, j = 0, k = 0;

    while (i < n1 && j < n2) {
        if (array1[i] < array2[j])
            result[k++] = array1[i++];
        else
            result[k++] = array2[j++];
    }

    while (i < n1)
        result[k++] = array1[i++];
    
    while (j < n2)
        result[k++] = array2[j++];
    
    return result;
    
}


int *generate_random_array(unsigned int random_seed, unsigned int length) {
    int *array = (int*)malloc(length * sizeof(int));
    srand(random_seed);
    for(int i=0; i < length; i++) { array[i] = rand(); }
    return array;
}


int main(int argc, char **argv){
    
    const unsigned int length = 1 << 20;
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int random_seed = strtoul(argv[2], NULL, 10);

    const unsigned int part_length = length / threads;

    int *array = generate_random_array(random_seed, length);

    int *test = (int*)malloc(length * sizeof(int));
    memcpy(test, array, length * sizeof(int));

    double time_start = omp_get_wtime();

    #pragma omp parallel num_threads(threads) default(none) shared(array, threads, part_length)
    {
        int i = omp_get_thread_num();
        shellsort(array + i * part_length, part_length);

        #pragma omp barrier
        
        for (int j = 1; j < threads; j <<= 1) {
            if (i % (j << 1) == 0) {
                int block_length = j * part_length;
                int *base = array + block_length * (i / j);
                
                int *temp = merge(base, base + block_length, block_length, block_length);
                memcpy(base, temp, sizeof(int) * (block_length << 1));
                free(temp);
            }
            #pragma omp barrier
        }
    }

    double time_end = omp_get_wtime();

    assert(is_sorted(array, length));

    shellsort(test, length);
    assert(memcmp(array, test, length * sizeof(int)) == 0);

    printf("%g\n", time_end - time_start);
    
    free(array);
    free(test);

    return 0;
}
