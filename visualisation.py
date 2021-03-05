import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

data = pd.read_csv('./cmake-build-release/res.csv')
x = data['size'].unique()
ys_chunks = []
legend_chunks = []
for name in data['name'].unique():
    df = data[data['name'] == name]
    df = df.drop(columns=['name'])
    ys = []
    legend = []
    for concurrency in df['concurrency'].unique():
        dfc = df[df['concurrency'] == concurrency]
        dfc = dfc.drop(columns=['concurrency'])
        legend.append(f'{name}_concurrency_{concurrency}')
        y = np.array(dfc['time'])
        ys.append(y)
    ys_chunks.append(ys)
    legend_chunks.append(legend)


i = 0
for ys_chunk, legend_chunk in zip(ys_chunks[1:], legend_chunks[1:]):
    i += 1
    plt.xlabel('collection size')
    plt.ylabel('time')
    plt.plot(x, ys_chunks[0][0])
    for y in ys_chunk:
        plt.plot(x, y)
    plt.legend(legend_chunks[0] + legend_chunk, loc=2)
    plt.savefig(f'./chunk{i}.png')
    plt.clf()