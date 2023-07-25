# This is a port of myreadv function from Matlab to Python
# Needed since my Matlab free trial got expired
# Author: Gabriel Andrade (gandrademachine), Rio de Janeiro State U. (gabrielandradedesign@gmail.com)

import os
import numpy as np
import myread

def myreadv(fname):
    v = myread.myread(fname)
    v = np.reshape(v,(3,int(len(v)/3)))
    return np.transpose(v)

#print('testing: ')
#myreadv(os.path.expanduser(r'~/rec-3dcurve-000243-points.dat'))
