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