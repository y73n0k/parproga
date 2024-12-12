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
