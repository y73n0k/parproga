from utils import *

def func(x):
    return x


xs, practical_time = load_time()
theoretical_time = get_theoretical_time(practical_time[0], xs, func)

practical_acceleration = get_practical_acceleration(practical_time)
theoretical_acceleration = get_theoretical_acceleration(xs, func)

practical_efficiency = get_practical_efficiency(practical_time)
theoretical_efficiency = get_theoretical_efficiency(xs, func)


fig, ax = init_subplots()
draw_graph(ax, xs, practical_time, "Среднее время", "Время, с")
draw_graph(ax, xs, theoretical_time, "Теоритическое среднее время", "Время, с")
save_fig(fig, ax, "pics/time.png")

fig, ax = init_subplots()
draw_graph(ax, xs, practical_acceleration, "Ускорение", "")
draw_graph(ax, xs, theoretical_acceleration, "Теоритическое ускорение", "")
save_fig(fig, ax, "pics/acceleration.png")

fig, ax = init_subplots()
draw_graph(ax, xs, practical_efficiency, "Эффективность", "")
draw_graph(ax, xs, theoretical_efficiency, "Теоритическая эффективность", "")
save_fig(fig, ax, "pics/efficiency.png")

#ДОЙКИ