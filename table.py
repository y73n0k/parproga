from utils import *
import pandas as pd


xs, p_time = load_time()
t_time = get_theoretical_time(p_time[0], xs)

pa = get_practical_acceleration(p_time)
ta = get_theoretical_acceleration(xs)

pe = get_practical_efficiency(p_time)
te = get_theoretical_efficiency(xs)

d = [
    {
        "Время": p_time,
        "Теоретическое время": t_time
    },
    {
        "Ускорение": pa,
        "Теоретическое ускорение": ta
    },
    {
        "Эффективность": pe,
        "Теоретическая эффективность": te,
    }
]

for i in range(len(d)):
    df = pd.DataFrame(d[i], index=xs)

    df.index.name = "Число потоков"
    df.to_latex(f"table{i}.tex")


#БУФЕРА