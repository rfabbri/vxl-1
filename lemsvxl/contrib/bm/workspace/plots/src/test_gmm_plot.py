'''
Created on Jun 4, 2011

@author: bm
'''
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from multivariate_normal import *
import pypr.clustering.gmm as gmm
sm_result_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_experiment1/sm_0/";

nframes = 252;

delta = 0.0025
xmin = -1.0;
xmax = 1.0;
ymin = -1.0;
ymax = 1.0;

grid_x = np.arange(xmin,xmax,delta);
grid_y = np.arange(ymin,ymax,delta);
X, Y = np.meshgrid(grid_x, grid_y);

coords =np.array([np.ravel(X).T, np.ravel(Y).T]).T

frame = 1;

weight_name = sm_result_dir + "weight_%04d.txt" % frame;
mean_name =  sm_result_dir + "mean_%04d.txt" % frame;
covar_name = sm_result_dir + "covar_%04d.txt" % frame;
observation_name = sm_result_dir + "observation_%04d.txt" % frame;
figure_name = sm_result_dir + "gmm_%04d.png" % frame;

weight_matrix = np.genfromtxt(weight_name, dtype = 'float');
mean_matrix = np.genfromtxt(mean_name, dtype = 'float')
covar_matrix = np.genfromtxt(covar_name, dtype = 'float')
current_observation = np.genfromtxt(observation_name, dtype = 'float')

if mean_matrix.ndim == 1:
    num_nodes = 1;
else:
    num_nodes = mean_matrix.shape[0];
               
m = [];
c = [];
w = []; 
         
for i in range(num_nodes):
    if num_nodes == 1:
        m.append( np.array([mean_matrix[0],mean_matrix[1]]) )
        c.append(covar_matrix);
        w.append(weight_matrix[1]);
    else:
        m.append(mean_matrix[i,1:3])
        c.append(covar_matrix[i*2:i*2+2,:]);
        w.append(weight_matrix[i,1]);

gmmpdf = gmm.gmm_pdf(coords,m,c,w);    

gmmpdf = np.reshape(gmmpdf, X.shape);

fig = plt.figure();
ax = fig.add_subplot(111,projection='3d');
fig.imshow(gmmpdf, cmap = 'jet',extent = [xmin,xmax,ymin,ymax])


#plt.contour(X,Y,gmmpdf)
plt.show()





