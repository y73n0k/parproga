from utils import *
import pandas as pd


def func(p):
    return p

BASE = "dump/1000000/"

xs_static, practical_time_static = load_time(BASE + "static")
practical_acceleration_static = get_practical_acceleration(practical_time_static)
practical_efficiency_static = get_practical_efficiency(practical_time_static)

xs_dynamic, practical_time_dynamic = load_time(BASE + "dynamic")
practical_acceleration_dynamic = get_practical_acceleration(practical_time_dynamic)
practical_efficiency_dynamic = get_practical_efficiency(practical_time_dynamic)

xs_guided, practical_time_guided = load_time(BASE + "guided")
practical_acceleration_guided = get_practical_acceleration(practical_time_guided)
practical_efficiency_guided = get_practical_efficiency(practical_time_guided)

theoretical_time = get_theoretical_time(practical_time_static[0], xs_static, func)
theoretical_acceleration = get_theoretical_acceleration(xs_static, func)
theoretical_efficiency = get_theoretical_efficiency(xs_static, func)


d = [
    {
        "Static": practical_time_static,
        "Dynamic": practical_time_dynamic,
        "Guided": practical_time_guided,
        "Теоретическое время": theoretical_time
    },
    {
        "Static": practical_acceleration_static,
        "Dynamic": practical_acceleration_dynamic,
        "Guided": practical_acceleration_guided,
        "Теоретическое ускорение": theoretical_acceleration
    },
    {
        "Static": practical_efficiency_static,
        "Dynamic": practical_efficiency_dynamic,
        "Guided": practical_efficiency_guided,
        "Теоретическое эффективность": theoretical_efficiency
    }
]

for i in range(len(d)):
    df = pd.DataFrame(d[i], index=xs_static)

    df.index.name = "Число потоков"
    df.to_latex(f"tables/table{i}.tex")


#БУФЕРА