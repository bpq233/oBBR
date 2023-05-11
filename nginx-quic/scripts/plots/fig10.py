import os
import csv
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np

plt.rc('font', size = 20) 
plt.rc('axes', grid = False)
plt.rc('axes', facecolor = 'white')
plt.rc('figure', figsize = (10, 5.5))
plt.rcParams['xtick.labelsize'] = 22 


current_directory = os.path.dirname(os.path.abspath(__file__))
out_dir=current_directory+'/../../figs/'
fig_format = '.png'
dir=current_directory+'/../../data/oBBR_fig10/'

bufs=['250KB', '500KB']
congs=['BBR', 'BBRv2', 'CUBIC']
BBR_U=['0.5', '0.75', '1.0']

goodput = {}

for cong in congs:
    for buf in bufs:
        for u in BBR_U:
            file1 = dir+"oBBR-"+u+'_vs_'+cong+'_'+buf+'_1'
            file2 = dir+"oBBR-"+u+'_vs_'+cong+'_'+buf+'_2'
            data1 = pd.read_csv(file1)
            Goodput1 = sum(data1.iloc[0:,1]) * 8 / len(data1) / 1024 / 1024
            data2 = pd.read_csv(file2)
            Goodput2 = sum(data2.iloc[0:,1]) * 8 / len(data2) / 1024 / 1024
            if cong not in goodput:
                goodput[cong] = []
                goodput['oBBR_vs_'+cong] = []
            goodput[cong].append(Goodput2)
            goodput['oBBR_vs_'+cong].append(Goodput1)
            
#print(goodput)

x=np.array([1,2,3,4,5,6])

width = 0.2  
fig,ax = plt.subplots()

ax.bar(x-0.2,goodput['oBBR_vs_BBR'],width,color='tab:blue',edgecolor='black')
ax.bar(x-0.2,goodput['BBR'],width,bottom=goodput['oBBR_vs_BBR'],label='BBR',color='r',edgecolor='black')


ax.bar(x,goodput['oBBR_vs_BBRv2'],width,color='tab:blue',edgecolor='black')
ax.bar(x,goodput['BBRv2'],width,hatch='--',bottom=goodput['oBBR_vs_BBRv2'],label='BBRv2',color='g',edgecolor='black')

ax.bar(x+0.2,goodput['oBBR_vs_CUBIC'],width,color='tab:blue',edgecolor='black',label='oBBR')
ax.bar(x+0.2,goodput['CUBIC'],width,bottom=goodput['oBBR_vs_CUBIC'],label='CUBIC',color='grey',edgecolor='black')

plt.xticks(x,["R:0.5\n$\mu:0.5$","R:0.5\n$\mu:0.75$","R:0.5\n$\mu:1.0$","R:1.0\n$\mu:0.5$","R:1.0\n$\mu:0.75$","R:1.0\n$\mu:1.0$"])

ax.set_ylabel('Goodput (Mbps)')
ax.set_ylim(0,55)
ax.legend(fontsize=20,ncol=2)

plt.savefig(out_dir+'fig10'+fig_format,bbox_inches = 'tight',dpi=300)

#plt.show()
            
