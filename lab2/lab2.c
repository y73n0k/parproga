#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int main(int argc, char** argv)
{   
    const int target = 1337;
    const int count = 20000000;
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int random_seed = strtoul(argv[2], NULL, 10);

    int *array = NULL;              ///< The array we need to find the max in
    int  index = -1;                ///< The index of the element we need

    /* Initialize the RNG */
    srand(random_seed);

    /* Generate the random array */
    array = (int*)malloc(count*sizeof(int));
    for(int i=0; i<count; i++) { array[i] = rand() % 100000; }

    double time_start = omp_get_wtime();

    /* Find the index of the element */
    #pragma omp parallel for num_threads(threads) shared(array, count, target) reduction(max: index) default(none)
    for(int i=0; i<count; i++)
    {
        if(array[i] == target)
        {
            index = i;
        }
    }   

    double time_end = omp_get_wtime();
    printf("Found occurence of %d at index %d;\n", target, index);
    printf("%g\n", time_end - time_start);

    return(0);
}
