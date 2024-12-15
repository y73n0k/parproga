#include <omp.h>
#include <stdio.h>

int main() {
    printf("_OPENMP - Версия OpenMP в формате yyyymm: %d\n", _OPENMP);
    printf("Дата принятия стандарта: %d/%d\n", _OPENMP % 100, _OPENMP / 100);
    printf("omp_get_num_procs() - Число процессоров, доступных для использования на момент вызова: %d\n", omp_get_num_procs());
    printf("omp_get_max_threads() - Максимально допустимое число нитей для использования в следующей параллельной области: %d\n", omp_get_max_threads());
    printf("omp_get_dynamic() - Разрешено ли системе динамически изменять количество нитей, используемых для выполнения параллельной области: %d\n", omp_get_dynamic());
    printf("omp_get_wtick() - Разрешение таймера: %g секунд\n", omp_get_wtick());
    printf("omp_get_nested() - Разрешён ли вложенный параллелизм: %d\n", omp_get_nested());
    printf("omp_get_max_active_levels() - Максимальное число вложенных параллельных областей: %d\n", omp_get_max_active_levels());

    omp_sched_t kind;
    int chunk_size;

    omp_get_schedule(&kind, &chunk_size);
    printf("omp_get_schedule(&kind, &chunk_size) - kind - Тип распределения нагрузки: ");
    switch (kind & 0xf) {
        case omp_sched_static:
            printf("static\n");
            break;
        case omp_sched_dynamic:
            printf("dynamic\n");
            break;
        case omp_sched_guided:
            printf("guided\n");
            break;
        case omp_sched_auto:
            printf("auto\n");
            break;
    }
    printf("omp_get_schedule(&kind, &chunk_size) - chunk_size - Количество итераций потоку за раз: %d\n", chunk_size);
}
