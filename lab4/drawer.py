from utils import *

def func(p):
    return p

xs_static, practical_time_static = load_time("dump/1000000/static")
practical_acceleration_static = get_practical_acceleration(practical_time_static)
practical_efficiency_static = get_practical_efficiency(practical_time_static)

xs_dynamic, practical_time_dynamic = load_time("dump/1000000/dynamic")
practical_acceleration_dynamic = get_practical_acceleration(practical_time_dynamic)
practical_efficiency_dynamic = get_practical_efficiency(practical_time_dynamic)

xs_guided, practical_time_guided = load_time("dump/1000000/guided")
practical_acceleration_guided = get_practical_acceleration(practical_time_guided)
practical_efficiency_guided = get_practical_efficiency(practical_time_guided)

theoretical_time = get_theoretical_time(practical_time_static[0], xs_static, func)
theoretical_acceleration = get_theoretical_acceleration(xs_static, func)
theoretical_efficiency = get_theoretical_efficiency(xs_static, func)


fig, ax = init_subplots()
draw_graph(ax, xs_static, theoretical_time, "Теоритическое среднее время", "Время, с")
draw_graph(ax, xs_static, practical_time_static, "Среднее время static", "Время, с")
draw_graph(ax, xs_dynamic, practical_time_dynamic, "Среднее время dynamic", "Время, с")
draw_graph(ax, xs_guided, practical_time_guided, "Среднее время guided", "Время, с")
save_fig(fig, ax, "pics/1000000/time.png")


fig, ax = init_subplots()
draw_graph(ax, xs_static, theoretical_acceleration, "Теоритическое ускорение", "")
draw_graph(ax, xs_static, practical_acceleration_static, "Ускорение static", "")
draw_graph(ax, xs_dynamic, practical_acceleration_dynamic, "Ускорение dynamic", "")
draw_graph(ax, xs_guided, practical_acceleration_guided, "Ускорение guided", "")
save_fig(fig, ax, "pics/1000000/acceleration.png")

fig, ax = init_subplots()
draw_graph(ax, xs_static, theoretical_efficiency, "Теоритическая эффективность", "")
draw_graph(ax, xs_static, practical_efficiency_static, "Эффективность static", "")
draw_graph(ax, xs_dynamic, practical_efficiency_dynamic, "Эффективность dynamic", "")
draw_graph(ax, xs_guided, practical_efficiency_guided, "Эффективность guided", "")
save_fig(fig, ax, "pics/1000000/efficiency.png")


#ДОЙКИ