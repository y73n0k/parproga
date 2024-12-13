from subprocess import Popen, PIPE
import matplotlib.pyplot as plt
from os import cpu_count
from os.path import exists
from random import randint
from pickle import dump, load


RUNS = 20
MAX_SYSTEM_THREADS = 8 #cpu_count()
MAX_TEST_THREADS = MAX_SYSTEM_THREADS + MAX_SYSTEM_THREADS // 2
MAX_INT = (1 << 32) - 1


def generate_seeds(same_seed: bool = False):
    if same_seed:
        return [randint(0, MAX_INT)] * RUNS
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


def get_theoretical_time(base_point, xs, func):
    d = get_theoretical_acceleration(xs, func)
    return [base_point / d[i] for i in range(len(xs))]


def get_theoretical_efficiency(xs, func):
    return [func(x) / x if x <= MAX_SYSTEM_THREADS else func(MAX_SYSTEM_THREADS) / x for x in xs]


def get_theoretical_acceleration(xs, func):
    return [func(x) if x <= MAX_SYSTEM_THREADS else func(MAX_SYSTEM_THREADS) for x in xs]


def get_practical_efficiency(time):
    base_point = time[0]
    return [base_point / (t * n) for n, t in enumerate(time, 1)]


def get_practical_acceleration(time):
    base_point = time[0]
    return [base_point / t for t in time]


def init_subplots():
    return plt.subplots()


def save_fig(fig, ax, save_name):
    ax.legend()
    fig.savefig(save_name)


def draw_graph(ax, xs, ys, xlabel, ylabel):
    ax.plot(xs, ys, label=xlabel)
    ax.set_xlabel("Число потоков")
    ax.set_ylabel(ylabel)


def draw_average_time(ax, practical_time, xs):
    theoretical_time = get_theoretical_time(practical_time[0], xs)
    draw_graph(ax, xs, practical_time, "Среднее время", "Время, с")
    draw_graph(ax, xs, theoretical_time, "Теоритическое среднее время", "Время, с")


def draw_efficiency(ax, practical_time, xs):
    theoretical_efficiency = get_theoretical_efficiency(xs)
    practical_efficiency = get_practical_efficiency(practical_time)
    draw_graph(ax, xs, practical_efficiency, "Эффективность", "")
    draw_graph(ax, xs, theoretical_efficiency, "Теоритическая эффективность", "")


def draw_acceleration(ax, practical_time, xs):
    theoretical_acceleration = get_theoretical_acceleration(xs)
    practical_acceleration = get_practical_acceleration(practical_time)
    draw_graph(ax, xs, practical_acceleration, "Ускорение", "")
    draw_graph(ax, xs, theoretical_acceleration, "Теоритическое ускорение", "")


def dump_time(obj, name="dump"):
    with open(name, "wb") as f:
        dump(obj, f)


def load_time(name="dump"):
    if exists(name):
        with open(name, "rb") as f:
            return load(f)
    return None

#БИДОНЫ