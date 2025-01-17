#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

int max(int *array, int len) {
    int max = array[0];
    for (int i = 1; i < len; ++i) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

int main(int argc, char** argv) {
    const unsigned long count = 20000000;
    const unsigned long random_seed = strtoul(argv[1], NULL, 10);

    int rank, size;
    int *array;
    int local_max = -1, global_max;
    int begin, finish;
    double start, middle_start, middle_end, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = count / size;
    int remainder = count % size;
    array = (int*)malloc(count * sizeof(int));

    if (!rank) {
        srand(random_seed);
        for (int i = 0; i < count; i++) {
            array[i] = rand();
        }
    }

    start = MPI_Wtime();
    MPI_Bcast(array, count, MPI_INT, 0, MPI_COMM_WORLD);
    middle_start = MPI_Wtime();

    begin = rank * chunk_size;
    finish = begin + chunk_size + (rank == size - 1 ? remainder : 0);

    for (;begin < finish; ++begin) {
        if (array[begin] > local_max) {
            local_max = array[begin];
        }
    }

    middle_end = MPI_Wtime();
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();

    MPI_Finalize();

    if (!rank) {
        printf("======\nMax is: %d;\n", global_max);
        printf("%g\n", middle_end - middle_start);
        printf("%g\n", end - start);
        assert(global_max == max(array, count));
        free(array);
    }

    return 0;
}
