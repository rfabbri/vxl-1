'''
Created on May 25, 2011

@author: bm
'''
import numpy as np
import matplotlib.pyplot as plt

results_dir = "C:/Users/bm/Documents/progressReview/westin/results/intensity_ratio_experiment1"
temporal_entropy_dat = results_dir +"/temporal_entropy.dat"
pivot_pixel_dat = results_dir + "/pivot_pixel_candidates.dat"
ncn_sptr_bin = results_dir + "/ncn_sptr.bin"


pivot_pixel_candidates = np.genfromtxt(pivot_pixel_dat);
pivot_pixel_candidates = pivot_pixel_candidates.transpose();

temporal_entropy = np.genfromtxt(temporal_entropy_dat);

plt.figure();
plt.imshow(temporal_entropy, cmap='gray');
plt.plot(pivot_pixel_candidates[0],pivot_pixel_candidates[1],'rx',markersize=10,label='neighbor candidate');
plt.legend(numpoints=1)
title_str = 'Set of %d Neighbor Candidates' % pivot_pixel_candidates.shape[1];
plt.title(title_str)
plt.colorbar()
plt.show()





