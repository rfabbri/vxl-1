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




target_colors = ['mo','ro','go','yo','co'];
neighbor_colors = ['mx','rx','gx','yx','cx'];

i=0;
for t_idx in range(0,neighborhoods.shape[0]-2,2):
    plt.subplot(2,2,i+1)
#    plt.figure();
    plt.imshow(temporal_entropy,cmap='gray');
    
    for n_idx in range(neighborhoods.shape[1]):
        plt.plot(neighborhoods[t_idx,n_idx],neighborhoods[t_idx+1,n_idx],neighbor_colors[i],markersize=10,linewidth=10)
        
    plt.plot(neighborhoods[t_idx,0],neighborhoods[t_idx+1,0],target_colors[i],markersize=10)
    plt.colorbar();
    title_string = "Westin Temporal Entropy, Target: (%d,%d) and Neighborhood" % (neighborhoods[t_idx,0],neighborhoods[t_idx,1]);
    plt.title(title_string)
#    plt.show();
    savename = results_dir+'/neighborhood%d.png' % i
#    plt.savefig(savename)
    i=i+1;

#for t_idx in range(0,neighborhoods.shape[0],2):
#    plt.plot(neighborhoods[t_idx,0],neighborhoods[t_idx,1],target_colors[t_idx%4],markersize=10)    

plt.show();
