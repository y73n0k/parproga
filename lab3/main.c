int main(int argc, char **argv){
    const unsigned int length = 1 << 22;
    unsigned int CHUNKS = 4;
    const unsigned int threads = strtoul(argv[1], NULL, 10);
    const unsigned int random_seed = strtoul(argv[2], NULL, 10);
    unsigned int part_size = length / CHUNKS;
    int *array = generate_random_array(random_seed, length);
    double time_start = omp_get_wtime();

    #pragma omp parallel for num_threads(threads) default(none) shared(array, threads, part_size, length, CHUNKS)
    for (unsigned int i = 0; i < CHUNKS; ++i) {
        int prev_size = i * part_size;
        int current_size = (i == CHUNKS - 1) ? (length - prev_size) : part_size;

        shellsort(array + prev_size, current_size);
    }

    while (CHUNKS > 0) {
        #pragma omp parallel for num_threads(threads) default(none) shared(array, threads, part_size, length, CHUNKS)
        for (int j = 0; j < CHUNKS; j += 1) {
            int start = j * (part_size << 1);
            int mid = start + part_size - 1;
            int end = (start + 2 * part_size - 1 < length) ? (start + 2 * part_size - 1) : (length - 1);

            if (mid < end) {
                merge(array, start, mid, end);
            }
        }
        part_size <<= 1;
        CHUNKS >>= 1;
    }

    double time_end = omp_get_wtime();
    assert(is_sorted(array, length));
    printf("%g\n", time_end - time_start);
    free(array);
    return 0;
}