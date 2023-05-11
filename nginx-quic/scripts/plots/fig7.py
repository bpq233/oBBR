import os
import csv
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np

plt.rc('font', size = 25) 
plt.rc('figure', figsize = (15, 6))
plt.rc('axes', grid = False)
plt.rc('axes', facecolor = 'white')
plt.rcParams['xtick.labelsize'] = 20 

current_directory = os.path.dirname(os.path.abspath(__file__))
_dir = current_directory+'/../../data/oBBR_fig7/'
out_dir=current_directory+'/../../figs/'
fig_format = '.png'

dirs = ['100Mbps_40ms_125KB_loss0%', '100Mbps_40ms_125KB_loss1%', '40Mbps_100ms_125KB_loss0%', '40Mbps_100ms_125KB_loss2%', 
        '60Mbps_60ms_90KB_loss0%', '60Mbps_60ms_180KB_loss0%', '60Mbps_60ms_270KB_loss0%']
files = ['BBR','B3R', 'BBR-S', 'oBBR-0.5', 'oBBR-0.75', 'oBBR-1.0','BBRv2','CUBIC']

goodput = {}
loss = {}



for dir in dirs:
    for file in files:
        data = pd.read_csv(_dir+dir+'/'+file)
        Goodput = sum(data.iloc[:,1]) * 8 / len(data) / 1024 / 1024
        Loss = data.iloc[:,7][len(data) - 1]
        if file not in goodput:
            goodput[file] = []
            loss[file] = []
        goodput[file].append(Goodput)
        loss[file].append(Loss)

        
x=np.array([1,2,3,4,5,6,7])
width = 0.1  # 条形图的宽度
labels=["100Mbps\n40ms\nR=0.25","100Mbps\n40ms\nR=0.25\n Loss=1%","40Mbps\n100ms\nR=0.25","40Mbps\n100ms\nR=0.25\n Loss=2%","60Mbps\n60ms\nR=0.2","60Mbps\n60ms\nR=0.4","60Mbps\n60ms\nR=0.6"]

plt.bar(x-0.35,loss[files[0]],width,label='BBR',color='red',edgecolor='black')
plt.bar(x-0.25,loss[files[1]],width,label='B3R',color='purple',hatch='\\\\',edgecolor='black')
plt.bar(x-0.15,loss[files[2]],width,label='BBR-S',color='orange',hatch='++',edgecolor='black')
plt.bar(x-0.05,loss[files[3]],width,label='oBBR-0.5',color='#66ccff',hatch='xx',edgecolor='black')
plt.bar(x+0.05,loss[files[4]],width,label='oBBR-0.75',color='#66ccff',hatch='..',edgecolor='black')
plt.bar(x+0.15,loss[files[5]],width,label='oBBR-1',color='#66ccff',hatch='//',edgecolor='black')
plt.bar(x+0.25,loss[files[6]],width,label='BBRv2',color='green',hatch='--',edgecolor='black')
plt.bar(x+0.35,loss[files[7]],width,label='CUBIC',color='grey',edgecolor='black')

plt.xticks(x,labels)

#plt.yscale('log')
plt.ylabel('Retransmission Ratio (%)')
plt.ylim(0,27)
#ax.set_title('Scores by group and gender')
plt.legend(fontsize=22,ncol=4)
plt.savefig(out_dir+'fig7.a'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()


plt.bar(x-0.35,goodput[files[0]],width,label='BBR',color='red',edgecolor='black')
plt.bar(x-0.25,goodput[files[1]],width,label='B3R',color='purple',hatch='\\\\',edgecolor='black')
plt.bar(x-0.15,goodput[files[2]],width,label='BBR-S',color='orange',hatch='++',edgecolor='black')
plt.bar(x-0.05,goodput[files[3]],width,label='oBBR-0.5',color='#66ccff',hatch='xx',edgecolor='black')
plt.bar(x+0.05,goodput[files[4]],width,label='oBBR-0.75',color='#66ccff',hatch='..',edgecolor='black')
plt.bar(x+0.15,goodput[files[5]],width,label='oBBR-1',color='#66ccff',hatch='//',edgecolor='black')
plt.bar(x+0.25,goodput[files[6]],width,label='BBRv2',color='green',hatch='--',edgecolor='black')
plt.bar(x+0.35,goodput[files[7]],width,label='CUBIC',color='grey',edgecolor='black')

colors = ['red','purple','orange','#66ccff','#66ccff','#66ccff','green','grey']
hatchs = [None,'\\\\','++','xx','..','//','--',None]

plt.xticks(x,labels)
plt.ylabel('Goodput (Mbps)')
plt.ylim(0,120)
#ax.set_title('Scores by group and gender')
plt.legend(fontsize=22,ncol=4)
plt.savefig(out_dir+'fig7.b'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()

