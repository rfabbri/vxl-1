'''
Created on Jun 3, 2011

@author: bm
'''
import numpy as np

def multivariate_normal(pt,mean,cov):
    part1 = np.exp(-0.5*2*np.log(2*np.pi))
    part2 = np.power(np.linalg.det(cov),-0.5)
    diff = pt - mean;
    part3 = np.exp(-0.5*np.dot(np.dot(diff.transpose(),np.linalg.inv(cov)),diff))
    normpdf = part1*part2*part3
    return normpdf;
