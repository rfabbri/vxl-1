'''
Created on May 23, 2011

@author: bm
'''

import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

results_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_experiment1";
pixel1 = results_dir + "/646_176_2.txt"
data = np.genfromtxt(pixel1,'float');
datat = data.transpose();
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

#for pt_idx in range(data.shape[0]):
#ax.scatter(data[:][0],data[:][1],data[:][2],c='r',marker='o');
ax.scatter(datat[0,:],datat[1,:],datat[2,:],c='r',marker='o');
#    print 'point = (%d,%d,%d)' % (data[pt_idx,0],data[pt_idx,1],data[pt_idx,2])
#    print 'idx = %d' % pt_idx
#    print 'data[%d,0] = %f' % (pt_idx, data[pt_idx, 0]);
#    print 'data[%d,1] = %f' % (pt_idx, data[pt_idx, 1]);
#    print 'data[%d,2] = %f' % (pt_idx, data[pt_idx, 2]);
    
ax.set_xlabel('Time')
ax.set_ylabel('PCA 1')
ax.set_zlabel('PCA 1')
#ax.set_title('Features at pixel (246,176)')

plt.show()    
    


