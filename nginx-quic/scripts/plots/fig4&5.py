import os
import csv
from matplotlib import pyplot as plt
import statsmodels.api as sm
import pandas as pd
import numpy as np

current_directory = os.path.dirname(os.path.abspath(__file__))
out_dir=current_directory+'/../../figs/'
fig_format = '.png'
dir = current_directory+'/../../data/oBBR_fig4&5/'

plt.rc('font', size = 20)
plt.rc('figure', figsize = (6, 3.5))
plt.rc('axes', grid = False)
plt.rc('axes', facecolor = 'white')

bw_ratio=[0.75,0.85,0.9,0.95,0.99,1.0]
rtt_ratio=[1.0,1.5,2.0,2.15,2.25,2.5]

files = ['20Mbps_rtt', '20-10Mbps_rtt', '20-5Mbps_rtt']

bw={}
rtt={}

for file in files:
    data = pd.read_csv(dir+file)
    bw_ratios = data.iloc[0:,3]
    rtt_ratios = data.iloc[0:,6]
    for x in bw_ratio:
        slen = 0
        max_len=0
        for i in range(len(bw_ratios)):
            if (bw_ratios[i] < x):
                slen=slen+1
            else:
                slen=0
            if (slen > max_len):
                max_len=slen
        if file not in bw:
            bw[file]=[]
        bw[file].append(max_len)
    
    for x in rtt_ratio:
        slen = 0
        max_len=0
        for i in range(len(rtt_ratios)):
            if (rtt_ratios[i] >= x):
                slen=slen+1
            else:
                slen=0
            if (slen > max_len):
                max_len=slen
        if file not in rtt:
            rtt[file]=[]
        rtt[file].append(max_len)

    
labels = ['0.75','0.85','0.9', '0.95','0.99', '1.0']
x=np.array([1,2,3,4,5,6])
width = 0.2  
plt.bar(x-width,bw[files[0]],width,label='constant Bw.')
plt.bar(x,bw[files[1]],width,color='r',label='drop to 10Mbps')
plt.bar(x+width,bw[files[2]],width,color='g',label='drop to 5Mbps')

plt.ylabel('max number of\nconsecutive sample')
plt.xlabel('Bandwidth of samples\nnormalized to the estimate')
plt.yscale('log')

plt.legend(loc=2,fontsize=17)
plt.xticks(x,labels)
plt.savefig(out_dir+'fig5.b'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()

labels = ['1.0','1.5','2.0', '2.15','2.25', '2.5']

plt.bar(x-width,rtt[files[0]],width,label='constant Bw.')
plt.bar(x,rtt[files[1]],width,color='r',label='drop to 10Mbps')
plt.bar(x+width,rtt[files[2]],width,color='g',label='drop to 5Mbps')

plt.ylabel('max number of\nconsecutive sample')
plt.xlabel('RTT of samples\nnormalized to the estimate')
plt.yscale('log')

plt.legend(loc=1,fontsize=17)
plt.xticks(x,labels)
plt.savefig(out_dir+'fig5.c'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()


data1 = pd.read_csv(dir+'20Mbps_rtt')
data = data1.iloc[0:,1]
data = data / 20 / 1024 / 1024
ecdf = sm.distributions.ECDF(data)
x = np.linspace(min(data), max(data))
y = ecdf(x)
plt.plot(x, y, linewidth=2, color="green", linestyle="dashed")


plt.ylabel('CDF', fontsize=20)
plt.xticks(fontsize=20)
plt.yticks(fontsize=20)
plt.xlabel('Bandwidth of samples\nnormalized to the link capacity', fontsize=20)

plt.savefig(out_dir+'fig5.a'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()



def find_k(lst, max):
    for i, num in enumerate(lst):
        if num > max:
            return i
    return -1

import matplotlib.gridspec
plt.rc('figure', figsize = (10, 4.5))
gs = matplotlib.gridspec.GridSpec(7,2)  
fig = plt.figure()
ax1 = fig.add_subplot(gs[0:2,0])

data = pd.read_csv(dir+'L_20-5Mbps')

c = 30

x1 = data.iloc[5:c,0]
y1 = data.iloc[5:c,2]

x1 = x1 / 1000
y1 = y1 / 1024
ax1.set_ylim([0, 300])
ax1.set_ylabel('Loss (KB)')
ax1.set_title('20Mbps to 5Mbps')
ax1.bar(x1,y1,color='r')
ax1.get_xaxis().set_visible(False)

ax2 = fig.add_subplot(gs[2:,0])
data = pd.read_csv(dir+'L_20-5Mbps_rtt')

k = find_k(data.iloc[:,0].tolist(), 5000)
k2 = find_k(data.iloc[:,0].tolist(), c * 1000)

x1 = data.iloc[k:k2,0]
y1 = data.iloc[k:k2,4]
y2 = data.iloc[k:k2,7]

x1 = x1 / 1000
y2 = y2 / 1024
ax2.set_ylim([0, 2500])
ax2.plot(x1, y1,'b',label="RTT")
ax2.plot([12,12],[0,1900],'r')
ax2.text(7,300,'  Bw.\ndrops',color='r',fontsize=16)
ax2.legend(loc=1,fontsize=18)
ax2.set_ylabel('RTT (ms)')
ax3 = ax2.twinx() # this is the important function
ax3.set_ylim([0, 700])
ax3.plot(x1, y2, 'g',label = "Inflight")
ax3.axis('off')
ax3.legend(loc=2,fontsize=18)
ax3.set_ylabel('Inflight (KB)')
ax2.set_xlabel('Time(s)')

ax4 = fig.add_subplot(gs[0:2,1])
data = pd.read_csv(dir+'L_20-2Mbps')


x1 = data.iloc[5:c,0]
y1 = data.iloc[5:c,2]

x1 = x1 / 1000
y1 = y1 / 1024
ax4.set_ylim([0, 300])
ax4.set_title('20Mbps to 2Mbps')
ax4.set_ylabel('Loss (KB)')
ax4.bar(x1,y1,color='r')
ax4.get_yaxis().set_visible(False)
ax4.get_xaxis().set_visible(False)
 
    
ax5 = fig.add_subplot(gs[2:,1])
data = pd.read_csv(dir+'L_20-2Mbps_rtt')

k = find_k(data.iloc[:,0].tolist(), 5000)
k2 = find_k(data.iloc[:,0].tolist(), c * 1000)

x1 = data.iloc[k:k2,0]
y1 = data.iloc[k:k2,4]
y2 = data.iloc[k:k2,7]

x1 = x1 / 1000
y2 = y2 / 1024
ax5.set_ylim([0, 2500])
ax5.plot(x1, y1,'b',label="RTT")
ax5.plot([12,12],[0,1900],'r')
ax5.text(5,300,'  Bw.\ndrops',color='r',fontsize=16)
ax5.legend(loc=1,fontsize=18)
ax5.get_yaxis().set_visible(False)
ax5.set_ylabel('RTT (ms)')
ax6 = ax5.twinx() # this is the important function
ax6.set_ylim([0, 700])
ax6.plot(x1, y2, 'g',label = "Inflight")
ax6.legend(loc=2,fontsize=18)
ax6.set_ylabel('Inflight (KB)')
ax5.set_xlabel('Time(s)')


plt.savefig(out_dir+'fig4'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()
