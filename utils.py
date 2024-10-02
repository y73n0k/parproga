from subprocess import Popen, PIPE
import matplotlib.pyplot as plt
from os import cpu_count
from os.path import exists
from random import randint
from pickle import dump, load


RUNS = 20
MAX_SYSTEM_THREADS = cpu_count()
MAX_TEST_THREADS = MAX_SYSTEM_THREADS + MAX_SYSTEM_THREADS // 2
MAX_INT = (1 << 32) - 1


def generate_seeds():
    return [randint(0, MAX_INT) for _ in range(RUNS)]


def run_lab(path, nthreads, seed):
    # Программа должна из аргументов принимать на вход количество потоков и сид
    proc = Popen(f"{path} {nthreads} {seed}", shell=True, stdout=PIPE)
    proc.wait()
    return float(proc.stdout.readlines()[-1])


def get_practical_time(path, nthreads, seeds):
    result = 0
    print(f"Started {nthreads}")
    for seed in seeds:
        result += run_lab(path, nthreads, seed)
    print(f"Done {nthreads}")
    return result / RUNS


def get_theoretical_time(base_point, xs):
    return [base_point / min(i, MAX_SYSTEM_THREADS) for i in range(1, len(xs) + 1)]


def get_theoretical_efficiency(xs):
    return [1 if x <= MAX_SYSTEM_THREADS else 1 / (x - MAX_SYSTEM_THREADS + 1) for  x in xs]


def get_theoretical_acceleration(xs):
    return [x if x <= MAX_SYSTEM_THREADS else MAX_SYSTEM_THREADS for x in xs]


def get_practical_efficiency(time):
    base_point = time[0]
    return [base_point / (t * n) for n, t in enumerate(time, 1)]


def get_practical_acceleration(time):
    base_point = time[0]
    return [base_point / t for t in time]


def draw_graph(xs, y1, y2, label1, label2, ylabel, save_name):
    fig, ax = plt.subplots()
    ax.plot(xs, y1, label=label1)
    ax.plot(xs, y2, label=label2)
    ax.set_xlabel("Число потоков")
    ax.set_ylabel(ylabel)
    ax.legend()

    fig.savefig(save_name)


def draw_average_time(practical_time, xs):
    theoretical_time = get_theoretical_time(practical_time[0], xs)
    draw_graph(xs, practical_time, theoretical_time, "Среднее время", "Теоритическое среднее время", "Время, с", "pics/time")


def draw_efficiency(practical_time, xs):
    theoretical_efficiency = get_theoretical_efficiency(xs)
    practical_efficiency = get_practical_efficiency(practical_time)
    draw_graph(xs, practical_efficiency, theoretical_efficiency, "Эффективность", "Теоритическая эффективность", "", "pics/efficiency")


def draw_acceleration(practical_time, xs):
    theoretical_acceleration = get_theoretical_acceleration(xs)
    practical_acceleration = get_practical_acceleration(practical_time)
    draw_graph(xs, practical_acceleration, theoretical_acceleration, "Ускорение", "Теоритическое ускорение", "", "pics/acceleration")


def dump_time(obj):
    with open("dump", "wb") as f:
        dump(obj, f)


def load_time():
    if exists("dump"):
        with open("dump", "rb") as f:
            return load(f)
    return None

#БИДОНЫ