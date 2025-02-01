#include <omp.h>
#include <mpi.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define NPROC 8


char is_prime(long num) {
    if (num == 1) {
        return 0;
    }

    long bound = (long)sqrt(num);

    for (long i = 2; i <= bound; ++i) {
        if (num % i == 0) {
            return 0;
        }
    }

    return 1;
}


std::vector<long> prime_range(long N1, long N2) {
    std::vector<long> result;
    #pragma omp parallel shared(N1, N2, result) default(none)
    {
        std::vector<long> result_private;
        #pragma omp for
        for (long i = N1; i < N2; ++i) {
            if (is_prime(i)) {
                result_private.push_back(i);
            }
        }
        #pragma omp critical
        result.insert(result.end(), result_private.begin(), result_private.end());   
    }
    return result;
}


int main(int argc, char *argv[]) {
    const long N1 = strtol(argv[1], NULL, 10);
    const long N2 = strtol(argv[2], NULL, 10);

    const long range_length = N2 - N1 + 1;
    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Datatype range_type;
    MPI_Type_contiguous(2, MPI_LONG, &range_type);
    MPI_Type_commit(&range_type);

    omp_set_dynamic(0);
    omp_set_num_threads(NPROC / size);

    long *send_data = NULL;
    long recv_data[2] = {0, 0};

    const long range_length_per_proc = range_length / size;
    const long remider = range_length % size;

    double timeit_start = MPI_Wtime();

    if (!rank) {
        send_data = (long *)malloc(sizeof(void *) * size * 2);
        for (int i = 0; i < size; ++i) {
            send_data[2 * i] = N1 + i * range_length_per_proc;
            send_data[2 * i + 1] = N1 + (i + 1) * range_length_per_proc - 1;
        }
        send_data[2 * (size - 1) + 1] += remider;
    }

    MPI_Scatter(send_data, 1, range_type, recv_data, 1, range_type, 0, MPI_COMM_WORLD);

    double start = MPI_Wtime();
    auto res = prime_range(recv_data[0], recv_data[1]);
    double end = MPI_Wtime();

    size_t length = res.size(), total_length = 0, lengths[size] = {0};

    MPI_Gather(&length, 1, MPI_LONG_LONG, lengths, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    int recvcounts[size], displs[size];

    if (!rank) {
        for (int i = 0; i < size; ++i) {
            recvcounts[i] = (int)lengths[i];
            displs[i] = (int)total_length;
            total_length += lengths[i];
        }
    }

    std::vector<long> answer;
    answer.reserve(total_length);
    answer.resize(total_length);

    MPI_Gatherv(&res[0], length, MPI_LONG, &answer[0], recvcounts, displs, MPI_LONG, 0, MPI_COMM_WORLD);

    double time = end - start;
    double times[size];

    MPI_Gather(&time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    double timeit_end = MPI_Wtime();

    if (!rank) {
        for (int i = 0; i < size; ++i) {
            std::cout << times[i] << std::endl;
        }
        std::cout << timeit_end - timeit_start << std::endl;
        free(send_data);
    }

    MPI_Finalize();
    return 0;
}