import matplotlib.pyplot as plt

color_1 = 'plum'
color_2 = 'cornflowerblue'
color_3 = 'aquamarine'

background_color = '#0d1117'

TYPES_N = 2

# EnqueueDequeue
mutex_int_ed = [113, 304, 438, 471, 490]
exp_int_ed   = [157, 192, 201, 215, 213]
yield_int_ed = [162, 196, 221, 238, 257]

mutex_struct_ed = [114, 393, 495, 544, 582]
exp_struct_ed   = [158, 182, 204, 245, 228]
yield_struct_ed = [160, 196, 234, 247, 251]

# EnqueueDequeueOnce
mutex_int_edo = [77, 151, 186, 191]
exp_int_edo   = [59.9, 83.6, 81.5, 81.2]
yield_int_edo = [56.1, 86.7, 85.1, 86.1]

mutex_struct_edo = [80.5, 157, 185, 192]
exp_struct_edo   = [58, 84, 82.3, 82.9]
yield_struct_edo = [52.9, 84.1, 82.8, 87.1]

# EnqueueOnceDequeue
mutex_int_eod = [77.2, 142, 188, 442]
exp_int_eod   = [59.3, 168, 325, 644]
yield_int_eod = [57.6, 170, 355, 695]

mutex_struct_eod = [82.1, 147, 192, 339]
exp_struct_eod   = [59.3, 162, 338, 661]
yield_struct_eod = [53.8, 173, 359, 747]

ed_titles =  ["EnqueueDequeue with int", "EnqueueDequeue with MyStruct"]
edo_titles = ["EnqueueDequeueOnce with int", "EnqueueDequeueOnce with MyStruct"]
eod_titles = ["EnqueueOnceDequeue with int", "EnqueueOnceDequeue with MyStruct"]

ed_results  = [(mutex_int_ed, exp_int_ed, yield_int_ed), (mutex_struct_ed, exp_struct_ed, yield_struct_ed)]
edo_results = [(mutex_int_edo, exp_int_edo, yield_int_edo), (mutex_struct_edo, exp_struct_edo, yield_struct_edo)]
eod_results = [(mutex_int_eod, exp_int_eod, yield_int_eod), (mutex_struct_eod, exp_struct_eod, yield_struct_eod)]

threads_from_1 = [1, 2, 4, 8, 16]
threads_from_2 = [2, 4, 8, 16]

fig, (ed, edo, eod) = plt.subplots(3, TYPES_N, figsize=(12, 10))

fig.patch.set_facecolor(background_color)

plt.subplots_adjust(left=0.1,
                    bottom=0.1,
                    right=0.9,
                    top=0.9,
                    wspace=0.1,
                    hspace=0.4)

for i in range(TYPES_N):
    # EnqueueDequeue
    g1 = ed[i]
    g1.set_title(ed_titles[i], color='white')
    g1.set_xticks(threads_from_1)
    results_1 = ed_results[i]

    g1.plot(threads_from_1, results_1[0], marker='o', color=color_1, label="TwoMutexQueue")
    g1.plot(threads_from_1, results_1[1], marker='o', color=color_2, label="MsQueue with exp_backoff")
    g1.plot(threads_from_1, results_1[2], marker='o', color=color_3, label="MsQueue with yield_backoff")

    # EnqueueDequeueOnce
    g2 = edo[i]
    g2.set_title(edo_titles[i], color='white')
    g2.set_xticks(threads_from_2)
    results_2 = edo_results[i]

    g2.plot(threads_from_2, results_2[0], marker='o', color=color_1, label="TwoMutexQueue")
    g2.plot(threads_from_2, results_2[1], marker='o', color=color_2, label="MsQueue with exp_backoff")
    g2.plot(threads_from_2, results_2[2], marker='o', color=color_3, label="MsQueue with yield_backoff")

    # EnqueueOnceDequeue
    g3 = eod[i]
    g3.set_title(eod_titles[i], color='white')
    g3.set_xticks(threads_from_2)
    results_3 = eod_results[i]

    g3.plot(threads_from_2, results_3[0], marker='o', color=color_1, label="TwoMutexQueue")
    g3.plot(threads_from_2, results_3[1], marker='o', color=color_2, label="MsQueue with exp_backoff")
    g3.plot(threads_from_2, results_3[2], marker='o', color=color_3, label="MsQueue with yield_backoff")

    g3.set_xlabel("Number of threads")

    for g in (g1, g2, g3):
        g.grid(color='white', linestyle='--', linewidth=0.2)
        g.patch.set_alpha(0.0)

        if i == 0:
            g.set_ylabel("Time, milliseconds")

        g.spines['bottom'].set_color('white')
        g.spines['top'].set_color('silver')
        g.spines['right'].set_color('silver')
        g.spines['left'].set_color('white')
        g.spines['top'].set_lw(0.5)
        g.spines['right'].set_lw(0.5)
        g.xaxis.label.set_color('white')
        g.yaxis.label.set_color('white')
        g.tick_params(colors='white', which='both')

lines_labels = [ax.get_legend_handles_labels() for ax in fig.axes]
lines, labels = [sum(lol, []) for lol in zip(*lines_labels)]
fig.legend(lines[:3], labels[:3], loc='upper center')

# plt.show()

plt.savefig('data/graphic.png')
