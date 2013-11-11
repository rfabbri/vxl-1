'''
Created on Jun 3, 2011

@author: bm
'''
import numpy as np
import matplotlib.pyplot as plt
from multivariate_normal import *
import pypr.clustering.gmm as gmm
sm_result_dir = "C:/Users/bm/Documents/progressReview/westin/results/sift_experiment1/sm_0/";

nframes = 252;

delta = 0.0025
xmin = -2.0;
xmax = 2.0;
ymin = -2.0;
ymax = 2.0;

grid_x = np.arange(xmin,xmax,delta);
grid_y = np.arange(ymin,ymax,delta);
X, Y = np.meshgrid(grid_x, grid_y);
#Xr = np.ravel(X);
#Yr = np.ravel(Y);

#coords = np.array([Xr.T,Yr.T]).T
coords =np.array([np.ravel(X).T, np.ravel(Y).T]).T

observations=np.zeros((nframes,2));

for frame in range(nframes):
    
    print 'plotting frame %d' % frame
    
    weight_name = sm_result_dir + "weight_%04d.txt" % frame;
    mean_name =  sm_result_dir + "mean_%04d.txt" % frame;
    covar_name = sm_result_dir + "covar_%04d.txt" % frame;
    observation_name = sm_result_dir + "observation_%04d.txt" % frame;
    figure_name = sm_result_dir + "gmm_%04d.png" % frame;
    
    weight_matrix = np.genfromtxt(weight_name, dtype = 'float');
    mean_matrix = np.genfromtxt(mean_name, dtype = 'float')
    covar_matrix = np.genfromtxt(covar_name, dtype = 'float')
    current_observation = np.genfromtxt(observation_name, dtype = 'float')
    
    observations[frame,0] = current_observation[0];
    observations[frame,1] = current_observation[1];
        
    
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
    
    plt.figure(0)
    plt.clf();
#    plt.hold(True)
    plt.imshow(gmmpdf,cmap='jet', extent = [xmin,xmax,ymin,ymax])
#    plt.contour(X,Y,gmmpdf)
    if frame == 0:
        plt.scatter(observations[0,0],observations[0,1],color='g',marker='d')
    else:
        plt.scatter(observations[range(frame),0],observations[range(frame),1],color='m',marker='d')
        plt.scatter(observations[frame,0],observations[frame,1],color='g',marker='d')
        
#    for i in range(len(w)):
#        x1, x2 = gmm.gauss_ellipse_2d(m[i],c[i])
#        plt.plot(x1,x2,'k',linewidth=2)
#    plt.plot(observations[range(frame+1),0],observations[range(frame+1),1],color='r',marker='D',markersize=12,linewidths=0.0)
#    plt.show()
    plt.savefig(figure_name)
    
    



            

    
            