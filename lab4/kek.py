from utils import load_time

import os
BASE = "dump/"
kek = []
for dir1 in os.listdir(BASE):
    for file in os.listdir(BASE + dir1):
        file = BASE + dir1 + "/" + file
        kek += [min(load_time(file)[1])]
print(min(kek))