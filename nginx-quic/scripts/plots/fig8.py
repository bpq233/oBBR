import os
import csv
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np
plt.rc('font', size = 25) #size为字体的大小
plt.rc('figure', figsize = (8, 5))
plt.rc('axes', grid = False)
plt.rc('axes', facecolor = 'white')

congs=['BBR', 'oBBR']

current_directory = os.path.dirname(os.path.abspath(__file__))
out_dir=current_directory+'/../../figs/'
fig_format = '.png'
dir=current_directory+'/../../data/oBBR_fig8/'

con = {}

def find_k(lst, max):
    for i, num in enumerate(lst):
        if num > max:
            return i
    return -1

for cong in congs:
    file1 = dir+cong
    file2 = dir+cong+'_rtt'
    data1 = pd.read_csv(file1)
    data2 = pd.read_csv(file2)
    con[cong+'_time'] = data1.iloc[:,0] / 1000
    con[cong+'_gp'] = data1.iloc[:,1] * 8 / 1024 / 1024
    con[cong+'_re'] = data1.iloc[:,5] / 1024 / 1024
    con[cong+'_bw'] = data2.iloc[:,2] * 8 / 1024 / 1024
    con[cong+'_xrtt'] = data2.iloc[:,0] / 1000
    con[cong+'_rtt'] = data2.iloc[:,4]
    
k1 = find_k(con['BBR_xrtt'].tolist(), 60)
k2 = find_k(con['oBBR_xrtt'].tolist(), 60)

real = []
for i in range(k1):
    if((con['BBR_xrtt'][i]) // 10 % 2 == 0):
        real.append(40)
    else:
        real.append(10)

plt.plot(con['BBR_xrtt'][0:k1],real,label='Real',color='black')
plt.plot(con['BBR_xrtt'][0:k1],con['BBR_bw'][0:k1],label='BBR',color='r')
plt.plot(con['oBBR_xrtt'][0:k2],con['oBBR_bw'][0:k2],label='oBBR',color='tab:blue')

plt.ylabel('Bandwidth (Mbps)')  
plt.xlabel('Time (s)') 
plt.ylim(0,56)
plt.legend(ncol=3,fontsize=21)
plt.savefig(out_dir+'fig8.a'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()

k1 = find_k(con['BBR_xrtt'].tolist(), 6)
k2 = find_k(con['oBBR_xrtt'].tolist(), 6)

k3 = find_k(con['BBR_xrtt'].tolist(), 18)
k4 = find_k(con['oBBR_xrtt'].tolist(), 18)

plt.plot(con['BBR_xrtt'][k1:k3],con['BBR_rtt'][k1:k3],label='BBR',color='r')
plt.plot(con['oBBR_xrtt'][k2:k4],con['oBBR_rtt'][k2:k4],label='oBBR',color='tab:blue')

plt.ylabel('RTT (s)')  
plt.xlabel('Time (s)') 
plt.legend(fontsize=21)
plt.savefig(out_dir+'fig8.b'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()

    
plt.plot(con['BBR_time'],con['BBR_re'],label='BBR',color='r')
plt.plot(con['oBBR_time'],con['oBBR_re'],label='oBBR',color='tab:blue')


plt.ylabel('Retransmissions (MB)')  
plt.xlabel('Time (s)') 
plt.legend(fontsize=21)
plt.savefig(out_dir+'fig8.c'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()


plt.plot(con['BBR_time'][0:120],con['BBR_gp'][0:120],label='BBR',color='r')
plt.plot(con['oBBR_time'][0:120],con['oBBR_gp'][0:120],label='oBBR',color='tab:blue')

plt.ylabel('Goodput (Mbps)')  
plt.xlabel('Time (s)') 
plt.ylim(0,50)
plt.legend(loc=1,ncol=2,fontsize=21)
plt.savefig(out_dir+'fig8.d'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()
