# This is a port of myread function from Matlab to Python
# Needed since my Matlab free trial got expired
# Author: Gabriel Andrade (gandrademachine), Rio de Janeiro State U. (gabrielandradedesign@gmail.com)

import os
import numpy as np

def myread(fname):
    fid = open(fname,'rb')
    data = np.fromfile(fid,float)
    fid.close()
    return data
#print('testing: ')
#myread(os.path.expanduser(r'~/rec-3dcurve-000243-points.dat'))
