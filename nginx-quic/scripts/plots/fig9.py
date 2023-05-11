import os
import csv
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np

plt.rc('font', size = 20)
plt.rc('figure', figsize = (11, 8))
plt.rc('axes', grid = False)
plt.rc('axes', facecolor = 'white')

plt.rcParams['axes.labelsize'] = 30 
plt.rcParams['xtick.labelsize'] = 30  
plt.rcParams['ytick.labelsize'] = 30  

current_directory = os.path.dirname(os.path.abspath(__file__))
out_dir=current_directory+'/../../figs/'
fig_format = '.png'
_dir = current_directory+'/../../data/oBBR_fig9/'
dirs = ['static1', 'static2', 'car1', 'car2']
files = ['BBR','B3R', 'BBR-S', 'oBBR-0.5', 'oBBR-0.75', 'oBBR-1.0','BBRv2','CUBIC']
out_file = ['fig9.a', 'fig9.b', 'fig9.c', 'fig9.d']

goodput = {}
loss = {}



for dir in dirs:
    for file in files:
        data = pd.read_csv(_dir+dir+'/'+file)
        Goodput = sum(data.iloc[:,1]) * 8 / len(data) / 1024 / 1024
        Loss = data.iloc[:,7][len(data) - 1]
        if dir not in goodput:
            goodput[dir] = []
            loss[dir] = []
        goodput[dir].append(Goodput)
        loss[dir].append(Loss)

labels = ['BBR','B3R',  'BBR-S','oBBR-0.5','oBBR-0.75', 'oBBR-1', 'BBRv2', 'CUBIC']
titles= ['static1 & 0.25xBDP', 'static2 & 0.5xBDP', 'car1 & 0.75xBDP', 'car2 & 1xBDP']
width = 0.3 
x1_list = []
x2_list = []
for i in range(len(files)):
    x1_list.append(i)
    x2_list.append(i + width)

i = 0
for dir in dirs:

    fig, ax1 = plt.subplots()

    # 设置左侧Y轴对应的figure
    ax1.set_ylabel('Retransmission Ratio (%)')
    ax1.set_ylim(0, 17)
    p1=ax1.bar(x1_list, loss[dir], width=width, color='red', align='edge',label='Retransmission Ratio')
    ax1.set_xticks(np.array(x1_list)+width)
    ax1.set_xticklabels(labels,rotation=-45)  # 设置共用的x轴

    # 设置右侧Y轴对应的figure
    ax2 = ax1.twinx()
    ax2.set_ylabel('Goodput (Mbps)')
    ax2.set_ylim(0, 50)
    p2=ax2.bar(x2_list, goodput[dir], width=width, color='tab:blue', align='edge', tick_label=labels,label='Goodput')

    plt.legend(fontsize=22,handles = [p1,p2],ncol=2)
    plt.tight_layout()
    plt.title(titles[i],fontsize=36)
    plt.savefig(out_dir+out_file[i]+fig_format,bbox_inches = 'tight',dpi=300)
#    plt.show()
    plt.clf()
    i = i + 1
