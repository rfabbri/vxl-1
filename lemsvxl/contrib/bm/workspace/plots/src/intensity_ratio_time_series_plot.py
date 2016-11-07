'''
Created on Jun 1, 2011

@author: bm
'''
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

results_dir = "C:/Users/bm/Documents/progressReview/westin/results/intensity_ratio_experiment1"
pixel1 = results_dir + "/reduced_pixel_time_series_map/646_176_2.txt";
data = np.genfromtxt(pixel1,'float');
data = data.transpose();
fig = plt.figure(1);

ax  = fig.add_subplot(111, projection = '3d')

ax.scatter(data[0,:], data[1,:], data[2,:], c='g', marker = 'o')
ax.set_xlabel('Time');
ax.set_ylabel('PCA 1');
ax.set_zlabel('PCA 2');
ax.set_title('Intensity Ratio Principal Component (2) Features for Pixel (646,176)')

plt.figure(2);

plt.subplot(211)

plt.scatter(data[0,:],data[1,:],c='g',marker = 'o');
plt.xlabel('Time');
plt.ylabel('PCA 1')
plt.title('Intensity Ratio Time vs. First Principal Component')

plt.subplot(212)
plt.scatter(data[0,:],data[2,:],c='g',marker='o');
plt.xlabel('Time');
plt.ylabel('PCA 2')
plt.title('Intensity Ratio Time vs. Second Principal Component')

plt.show();

