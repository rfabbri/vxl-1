import auctionSimulator.hw4.agents as hw4_agents 
import itertools
import numpy
import multiprocessing

import os

def f(x):
    return x*x

def worker(x):
    return x


    
if __name__ == '__main__':
    global output
    
    print "Using {0} cpus".format(multiprocessing.cpu_count())
    pool = multiprocessing.Pool()
    
          
    result = pool.map(worker,xrange(1000000))

    pool.close()
    pool.join()
    
    print result
       