#include <stdio.h>
#include <omp.h>
#include <assert.h>

int main() {
    const int N = 1000;
    int array[N];
    int total_sum = 0;
    omp_lock_t lock;

    for (int i = 0; i < N; i++) { array[i] = 1337; }

    omp_init_lock(&lock);

    #pragma omp parallel
    {
        int partial_sum = 0;

        #pragma omp for
        for (int i = 0; i < N; i++) {
            partial_sum += array[i];
        }

        omp_set_lock(&lock);
        total_sum += partial_sum;
        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);

    printf("%d: %s\n", total_sum, 1337 * N == total_sum ? "true" : "false");
    return 0;
}
