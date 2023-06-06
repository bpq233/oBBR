import os
import csv
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np
import statistics

plt.rc('font', size = 24) 
plt.rc('figure', figsize = (15, 6))
plt.rc('axes', grid = False)
plt.rc('axes', facecolor = 'white')
plt.rcParams['xtick.labelsize'] = 24 

current_directory = os.path.dirname(os.path.abspath(__file__))
# _dir = current_directory+'/../../data/mul/'
_dir = current_directory+'/../../data/mul/'
out_dir=current_directory+'/../../figs/'
fig_format = '.png'

dirs = ['0_flows', '1_flows', '2_flows', '3_flows', '4_flows']
files = ['BBR','B3R', 'BBR-S', 'oBBR-0.5', 'oBBR-0.75', 'oBBR-1.0','BBRv2','CUBIC']
flows = [1, 2, 3, 4, 5]

goodput = {}
loss = {}


c = 0
for dir in dirs:
    for file in files:
        Goodput = []
        Loss = []
        for i in range(flows[c]):
            data = pd.read_csv(_dir+dir+'/'+file+'_'+str(i))
            Goodput.append(sum(data.iloc[:,1]) * 8 / len(data) / 1024 / 1024)
            Loss.append(data.iloc[:,7][len(data) - 1])
            if sum(data.iloc[:,1]) < 1024 * 1024 * 1024:
                print(dir+'-'+file+'_'+str(i)+' is not done')
        if file not in goodput:
            goodput[file] = []
            goodput[file+'std'] = []
            loss[file] = []
            loss[file+'std'] = []
        goodput[file].append(statistics.mean(Goodput))
        loss[file].append(statistics.mean(Loss))
        goodput[file+'std'].append(statistics.stdev(Goodput) if len(Goodput) > 1 else 0.0)
        loss[file+'std'].append(statistics.stdev(Loss) if len(Loss) > 1 else 0.0)
    c = c + 1


print(goodput)
print(loss)
        
x=np.array([1,2,3,4,5])
width = 0.1  # 条形图的宽度
labels=["1-flow","2-flows","3-flows","4-flows","5-flows"]

plt.bar(x-0.35,loss[files[0]],width,yerr=loss[files[0]+'std'],capsize=3,label='BBR',color='red',edgecolor='black')
plt.bar(x-0.25,loss[files[1]],width,yerr=loss[files[1]+'std'],capsize=3,label='B3R',color='purple',hatch='\\\\',edgecolor='black')
plt.bar(x-0.15,loss[files[2]],width,yerr=loss[files[2]+'std'],capsize=3,label='BBR-S',color='orange',hatch='++',edgecolor='black')
plt.bar(x-0.05,loss[files[3]],width,yerr=loss[files[3]+'std'],capsize=3,label='oBBR-0.5',color='#66ccff',hatch='xx',edgecolor='black')
plt.bar(x+0.05,loss[files[4]],width,yerr=loss[files[4]+'std'],capsize=3,label='oBBR-0.75',color='#66ccff',hatch='..',edgecolor='black')
plt.bar(x+0.15,loss[files[5]],width,yerr=loss[files[5]+'std'],capsize=3,label='oBBR-1',color='#66ccff',hatch='//',edgecolor='black')
plt.bar(x+0.25,loss[files[6]],width,yerr=loss[files[6]+'std'],capsize=3,label='BBRv2',color='green',hatch='--',edgecolor='black')
plt.bar(x+0.35,loss[files[7]],width,yerr=loss[files[7]+'std'],capsize=3,label='CUBIC',color='grey',edgecolor='black')

plt.xticks(x,labels)

#plt.yscale('log')
plt.ylabel('Avg. Retransmission\nRatio per flow (%)')
plt.ylim(0,24)
#ax.set_title('Scores by group and gender')
plt.legend(fontsize=22,ncol=4)
plt.savefig(out_dir+'mul-loss'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()


plt.bar(x-0.35,goodput[files[0]],width,yerr=goodput[files[0]+'std'],capsize=3,label='BBR',color='red',edgecolor='black')
plt.bar(x-0.25,goodput[files[1]],width,yerr=goodput[files[1]+'std'],capsize=3,label='B3R',color='purple',hatch='\\\\',edgecolor='black')
plt.bar(x-0.15,goodput[files[2]],width,yerr=goodput[files[2]+'std'],capsize=3,label='BBR-S',color='orange',hatch='++',edgecolor='black')
plt.bar(x-0.05,goodput[files[3]],width,yerr=goodput[files[3]+'std'],capsize=3,label='oBBR-0.5',color='#66ccff',hatch='xx',edgecolor='black')
plt.bar(x+0.05,goodput[files[4]],width,yerr=goodput[files[4]+'std'],capsize=3,label='oBBR-0.75',color='#66ccff',hatch='..',edgecolor='black')
plt.bar(x+0.15,goodput[files[5]],width,yerr=goodput[files[5]+'std'],capsize=3,label='oBBR-1',color='#66ccff',hatch='//',edgecolor='black')
plt.bar(x+0.25,goodput[files[6]],width,yerr=goodput[files[6]+'std'],capsize=3,label='BBRv2',color='green',hatch='--',edgecolor='black')
plt.bar(x+0.35,goodput[files[7]],width,yerr=goodput[files[7]+'std'],capsize=3,label='CUBIC',color='grey',edgecolor='black')

colors = ['red','purple','orange','#66ccff','#66ccff','#66ccff','green','grey']
hatchs = [None,'\\\\','++','xx','..','//','--',None]

plt.xticks(x,labels)
plt.ylabel('Avg. Goodput per flow (Mbps)')
plt.ylim(0,125)
#ax.set_title('Scores by group and gender')
plt.legend(fontsize=22,ncol=4)
plt.savefig(out_dir+'mul-good'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()

