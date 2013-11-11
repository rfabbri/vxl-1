'''
Created on May 25, 2011

@author: bm
'''
import numpy as np
import matplotlib.pyplot as plt

results_dir = "C:/Users/bm/Documents/progressReview/westin/results/intensity_ratio_experiment1"
temporal_entropy_dat = results_dir +"/temporal_entropy.dat"
neighborhoods_dat = results_dir + "/neighborhoods.dat";

temporal_entropy = np.genfromtxt(temporal_entropy_dat);

neighborhoods = np.genfromtxt(neighborhoods_dat);

ntargets = neighborhoods.shape[0]/2




#plt.plot(neighborhoods[0,0],neighborhoods[1,0],'x',label='target1')
#plt.plot(neighborhoods[0,1],neighborhoods[1,1],'x',label='target1_neighbor1')
#plt.plot(neighborhoods[0,2],neighborhoods[1,2],'x',label='target1_neighbor2')

target_colors = ['ko','ro','go','yo']
neighbor_colors = ['kx','rx','gx','yx']

i=0;
for t_idx in range(0,neighborhoods.shape[0],2):
#    plt.subplot(2,2,i+1)
    plt.figure();
    plt.imshow(temporal_entropy,cmap='binary');
    
    for n_idx in range(neighborhoods.shape[1]):
        plt.plot(neighborhoods[t_idx,n_idx],neighborhoods[t_idx+1,n_idx],neighbor_colors[i],markersize=10,linewidth=10)
        
    plt.plot(neighborhoods[t_idx,0],neighborhoods[t_idx+1,0],target_colors[i],markersize=10)
    title_string = "Westin Temporal Entropy and Neighborhood %d" % i
    plt.title(title_string)
#    plt.show();
    savename = results_dir+'/neighborhood%d.png' % i
    plt.savefig(savename)
    i=i+1;

#for t_idx in range(0,neighborhoods.shape[0],2):
#    plt.plot(neighborhoods[t_idx,0],neighborhoods[t_idx,1],target_colors[t_idx%4],markersize=10)    

plt.show();
