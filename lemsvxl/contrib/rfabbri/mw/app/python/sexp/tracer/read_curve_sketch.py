
# This is a port of myread function from Matlab to Python
# Needed since my Matlab free trial got expired
# Author: Gabriel Andrade (gandrademachine), Rio de Janeiro State U. (gabrielandradedesign@gmail.com)
# this is a port from read_curve_sketch.m
import os
import subprocess
import sys
sys.path.append('../../utils')
import myreadv
import numpy as np

# Checks if reads the string file, if it doesn't, then exit
try:
    
    mypath = '~/'
    works = subprocess.check_output(f"ls {mypath}/*-3dcurve-*-points*dat", shell=True) # Checks if there are files in directory!
except subprocess.CalledProcessError as err:
    print(err)
    sys.exit(0)

data = ''.join(map(chr,works)) # Converts to a string
usable = [x for x in data.split('\n')]
usable.pop() # removing the empty one

# getting curves, convert to array, print size and create file
output_data = [myreadv.myreadv(i) for i in usable] 
output_data = np.array(output_data)
print('ncurves = ',len(output_data))
# Create output to plot
with open("all_rec_curves.txt","w") as rec:
    for crv in output_data:
        rec.write(''.join([str(el) for el in crv])+'\n')
