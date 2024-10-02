from sys import argv
from utils import *


if len(argv) != 2:
    print(f"usage: python {argv[0]} <path_to_file>")
    exit(1)

path = argv[1]
seeds = generate_seeds()

xs = list(range(1, MAX_TEST_THREADS + 1))

practical_time = load_time()
if practical_time is None:
    practical_time = [get_practical_time(path, i, seeds) for i in xs]
    dump_time([xs, practical_time])

#СИСЬКИ