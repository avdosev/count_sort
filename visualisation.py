import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

data = pd.read_csv('./cmake-build-release/res.csv')
x = data['size'].unique()
ys = []
legend = []
for name in data['name'].unique():
    df = data[data['name'] == name]
    df = df.drop(columns=['name'])
    for concurrency in df['concurrency'].unique():
        dfc = df[df['concurrency'] == concurrency]
        dfc = dfc.drop(columns=['concurrency'])
        legend.append(f'{name}_concurrency_{concurrency}')
        y = np.array(dfc['time'])
        ys.append(y)

def chunks(lst, chunk_size):
    return [lst[i:i+chunk_size] for i in range(0, len(lst), chunk_size)]

i = 0
for ys_chunk, legend_chunk in zip(chunks(ys[1:], 4), chunks(legend[1:], 4)):
    i += 1
    plt.xlabel('collection size')
    plt.ylabel('time')
    plt.plot(x, ys[0])
    for y in ys_chunk:
        plt.plot(x, y)
    plt.legend([legend[0]] + legend_chunk, loc=2)
    plt.savefig('./chunk'+str(i)+'.png')
    plt.clf()