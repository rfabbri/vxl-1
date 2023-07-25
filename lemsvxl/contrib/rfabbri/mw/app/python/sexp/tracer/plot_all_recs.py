# This is a port of myread function from Matlab to Python
# Needed since my Matlab free trial got expired
# Author: Gabriel Andrade (gandrademachine), Rio de Janeiro State U. (gabrielandradedesign@gmail.com)
# this is a port from read_curve_sketch.m
# Acknowledgements: This code was sponsored by Coordenacao de Aperfeicoamento de Pessoal de Ensino Superior (CAPES).Finantial code 001

import os
import subprocess
import sys
sys.path.append('../../utils')
import myreadv
import numpy as np
import matplotlib.pyplot as plt
import re, ast

#crvs = np.loadtxt("all_rec_curves.txt", delimiter=",")
crvs = []
with open("all_rec_curves.txt","r") as rec:
    f = rec.readlines()
    print(f)
    for r in rec:
        a =np.fromstring(r,dtype=float)
        #print(a)
        crvs.append(r)
print(crvs)
#ax = plt.figure().add_subplot(projection='3d')
#
#plt.title('Main reconstruction')
#plt.show()
