#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int main(int argc, char** argv)
{
    const int count = 20000000;
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int random_seed = strtoul(argv[2], NULL, 10);

    int* array = NULL;
    int  max   = -1;

    srand(random_seed);

    array = (int*)malloc(count*sizeof(int));
    for(int i=0; i<count; i++) { array[i] = rand(); }

    double time_start = omp_get_wtime();

    #pragma omp parallel num_threads(threads) shared(array, count) reduction(max: max) default(none)
    {
        #pragma omp for schedule(static)
        for(int i=0; i<count; i++)
        {
            if(array[i] > max) { max = array[i]; };
        }
    }

    double time_end = omp_get_wtime();
    printf("%g\n", time_end - time_start);

    return 0;
}
