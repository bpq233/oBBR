
import os
import csv
from matplotlib import pyplot as plt
import pandas as pd
import numpy as np

plt.rc('font', size = 20) 
plt.rc('figure', figsize = (5.5, 3.5))
plt.rc('axes', grid = False)
plt.rc('axes', facecolor = 'white')


current_directory = os.path.dirname(os.path.abspath(__file__))
out_dir=current_directory+'/../../figs/'
fig_format = '.png'
dir = current_directory+'/../../data/oBBR_fig3'

buffer=['50', '80', '100', '200', '1600']
R=['0.5', '0.8', '1', '2', '16']
cwnd_gain=['1.0', '1.5', '2.0', '3.0', '4.0']

cubic={}
bbr={}

start=60

for buf,r in zip(buffer,R):
    if buf == '1600':
        for g in cwnd_gain:
            file1 = dir+"/cubic_vs_bbr_g="+g+'_1600KB_1'
            file2 = dir+"/cubic_vs_bbr_g="+g+'_1600KB_2'
            data1 = pd.read_csv(file1)
            data2 = pd.read_csv(file2)
            Goodput1 = sum(data1.iloc[start:,1]) * 8 / len(data1.iloc[start:,1]) / 1024 / 1024
            Goodput2 = sum(data2.iloc[start:,1]) * 8 / len(data2.iloc[start:,1]) / 1024 / 1024
            if ('R=16' not in cubic):
                cubic['R=16'] = []
            cubic['R=16'].append(Goodput1)
            if ('R=16' not in bbr):
                bbr['R=16'] = []
            bbr['R=16'].append(Goodput2)

    file1 = dir+"/cubic_vs_bbr_g=1.0_"+buf+'KB_1'
    file2 = dir+"/cubic_vs_bbr_g=1.0_"+buf+'KB_2'
    data1 = pd.read_csv(file1)
    data2 = pd.read_csv(file2)
    Goodput1 = sum(data1.iloc[start:,1]) * 8 / len(data1.iloc[start:,1]) / 1024 / 1024
    Goodput2 = sum(data2.iloc[start:,1]) * 8 / len(data2.iloc[start:,1]) / 1024 / 1024
    if ('g=1.0' not in cubic):
        cubic['g=1.0'] = []
    cubic['g=1.0'].append(Goodput1)
    if ('g=1.0' not in bbr):
        bbr['g=1.0'] = []
    bbr['g=1.0'].append(Goodput2)

    file1 = dir+"/cubic_vs_bbr_g=2.0_"+buf+'KB_1'
    file2 = dir+"/cubic_vs_bbr_g=2.0_"+buf+'KB_2'
    data1 = pd.read_csv(file1)
    data2 = pd.read_csv(file2)
    Goodput1 = sum(data1.iloc[start:,1]) * 8 / len(data1.iloc[start:,1]) / 1024 / 1024
    Goodput2 = sum(data2.iloc[start:,1]) * 8 / len(data2.iloc[start:,1]) / 1024 / 1024
    if ('g=2.0' not in cubic):
        cubic['g=2.0'] = []
    cubic['g=2.0'].append(Goodput1)
    if ('g=2.0' not in bbr):
        bbr['g=2.0'] = []
    bbr['g=2.0'].append(Goodput2)

#print(bbr['g=1.0'], bbr['g=2.0'], bbr['R=16'])

width = 0.35 

labels = ['R=0.5', 'R=0.8', 'R=1', 'R=2', 'R=16']
fig,ax = plt.subplots()
ax.bar(labels,bbr['g=1.0'],width,label='BBR')
ax.bar(labels,cubic['g=1.0'],width,bottom=bbr['g=1.0'],label='CUBIC')

a=np.array([float(item) for item in bbr['g=1.0']])
b=np.array([float(item) for item in cubic['g=1.0']])
g=np.array([1,1,1,1,1])
R=np.array([float(item) for item in R])

points = (1.0-(R-g+1)/R/g)*(a+b)
point = np.minimum(points, (a+b))

ax.plot([0-width/2,width/2],[point[0],point[0]],'r',linewidth=5)
ax.plot([1-width/2,1+width/2],[point[1],point[1]],'r',linewidth=5)
ax.plot([2-width/2,2+width/2],[point[2],point[2]],'r',linewidth=5)
ax.plot([3-width/2,3+width/2],[point[3],point[3]],'r',linewidth=5)
ax.plot([4-width/2,4+width/2],[point[4],point[4]],'r',linewidth=5)

ax.set_ylabel('Goodput (Mbps)')
ax.set_ylim(0,27)
ax.set_title('g=1')
ax.legend(ncol=2,fontsize=20)
plt.savefig(out_dir+'fig3.b'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()

fig,ax = plt.subplots()
ax.bar(labels,bbr['g=2.0'],width,label='BBR')
ax.bar(labels,cubic['g=2.0'],width,bottom=bbr['g=2.0'],label='CUBIC')

a=np.array([float(item) for item in bbr['g=2.0']])
b=np.array([float(item) for item in cubic['g=2.0']])
g=np.array([2,2,2,2,2])
R=np.array([float(item) for item in R])

points = (1-(R-g+1)/R/g)*(a+b)
point = np.minimum(points, (a+b))

ax.plot([0-width/2,width/2],[point[0],point[0]],'r',linewidth=5)
ax.plot([1-width/2,1+width/2],[point[1],point[1]],'r',linewidth=5)
ax.plot([2-width/2,2+width/2],[point[2],point[2]],'r',linewidth=5)
ax.plot([3-width/2,3+width/2],[point[3],point[3]],'r',linewidth=5)
ax.plot([4-width/2,4+width/2],[point[4],point[4]],'r',linewidth=5)

ax.set_ylabel('Goodput (Mbps)')
ax.set_ylim(0,27)
ax.set_title('g=2')
ax.legend(ncol=2,fontsize=20)
plt.savefig(out_dir+'fig3.a'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()

fig,ax = plt.subplots()
labels = ['g=1', 'g=1.5', 'g=2', 'g=3', 'g=4']
ax.bar(labels,bbr['R=16'],width,label='BBR')
ax.bar(labels,cubic['R=16'],width,bottom=bbr['R=16'],label='CUBIC')

a=np.array([float(item) for item in bbr['R=16']])
b=np.array([float(item) for item in cubic['R=16']])
g=np.array([float(item) for item in cwnd_gain])
R=np.array([16,16,16,16,16])

points = (1-(R-g+1)/R/g)*(a+b)
point = np.minimum(points, (a+b))

ax.plot([0-width/2,width/2],[point[0],point[0]],'r',linewidth=5)
ax.plot([1-width/2,1+width/2],[point[1],point[1]],'r',linewidth=5)
ax.plot([2-width/2,2+width/2],[point[2],point[2]],'r',linewidth=5)
ax.plot([3-width/2,3+width/2],[point[3],point[3]],'r',linewidth=5)
ax.plot([4-width/2,4+width/2],[point[4],point[4]],'r',linewidth=5)

ax.set_ylabel('Goodput (Mbps)')
ax.set_ylim(0,27)
ax.set_title('R=16')
ax.legend(ncol=2,fontsize=20)
plt.savefig(out_dir+'fig3.c'+fig_format,bbox_inches = 'tight',dpi=300)
#plt.show()
plt.clf()
