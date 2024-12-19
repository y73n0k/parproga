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
    int *array, *local_array, *sendcounts, *displs;
    int local_max, global_max;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = count / size;
    int remainder = count % size;

    if (!rank) {
        srand(random_seed);

        sendcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));

        for (int i = 0; i < size; i++) {
            sendcounts[i] = chunk_size + (!i ? remainder : 0);
            displs[i] = i * chunk_size + remainder * (!i ? 0 : 1);
        }

        array = (int*)malloc(count * sizeof(int));
        for (int i = 0; i < count; i++) {
            array[i] = rand();
        }
    }

    local_array = (int*)malloc((chunk_size + (!rank ? remainder: 0)) * sizeof(int));

    start = MPI_Wtime();

    MPI_Scatterv(array, sendcounts, displs, MPI_INT, local_array, (chunk_size + (!rank ? remainder: 0)), MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk_size; i++) {
        if (local_array[i] > local_max) {
            local_max = local_array[i];
        }
    }

    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    end = MPI_Wtime();

    MPI_Finalize();

    if (!rank) {
        printf("======\nMax is: %d;\n", global_max);
        printf("%g\n", end - start);

        assert(global_max == max(array, count));
        free(sendcounts);
        free(displs);
        free(array);
    }

    return 0;
}
