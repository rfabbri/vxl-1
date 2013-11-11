"""
this is /auctionSimulator/hw/pricePredictions/symmetricPointPricePrediction.py

Author: Brandon Mayer
Date:   11/19/2011

Because the multiprocessing python module must be executed as a script
this is a command line interface script for generating Self Confirming 
Point Price Predicitons from Yoon & Wellman 2011 executing the each game
in parallel.
"""
from auctionSimulator.hw4.agents.baselineBidder import *
from auctionSimulator.hw4.agents.targetPrice import *
from auctionSimulator.hw4.agents.targetMV import *
from auctionSimulator.hw4.agents.targetMVS import *
from auctionSimulator.hw4.agents.straightMV import *

import numpy
import argparse
import multiprocessing
import time
import itertools
import matplotlib.pyplot as plt
import os

class symmetricPPP(object):
    """
    Functor to facilitate concurrency in symmetric self confirming price prediction (Yoon & Wellman 2011)
    
    Due to the homogeneity of the "games" defined in the self confirming price prediction,
    We can specify an agent type and the number of goods that will participate in all acutions.
    
    Therefore create an instance of this class with the correct parameters then we can use
    multiprocessing.Pool.map to pass a single argument to the callable class.
    
    When this class is called, it instantiates a new agent (so that each concurrent process
    is run with a completely different agent) and returns a unique bid instance.
    """
    def __init__(self,agentType = 'baselineBidder', m = 5):
        # cache values for instance specific initialization
        self.agentType = agentType
        self.m = m
            
            
    def __call__(self,pointPricePrediction):
        """
        Make the class callable with a single argument for multiprocessing.Pool.map()
        """
        agent = []
        
        if self.agentType == 'straightMV':
            agent = straightMV(m=self.m)
        elif self.agentType == 'targetMV':
            agent = targetMV(m=self.m)
        elif self.agentType == 'targetMVS':
            agent = targetMVS(m=self.m)
        else:
            agent = baselineBidder(m=self.m)    
                
        
        return numpy.array(agent.bid({'pricePrediction':pointPricePrediction})).astype('float64')

    

def main():
    desc = 'Asyncronious Self Confirmin Point Price Prediction (Yoon & Wellman 2011)'
    parser = argparse.ArgumentParser(description=desc)
    
    parser.add_argument('--agentType',     action='store', dest='agentType',         default='baselineBider', nargs='+')
    parser.add_argument('--allTypes',      action='store_true')
    
    parser.add_argument('--outDir',        action='store', dest='outDir', required=True)
    
    parser.add_argument('--m',             action='store', dest='m',     type=int,   default = 5)
    parser.add_argument('--L',             action='store', dest='L',     type=int,   default = 100)
    parser.add_argument('--d',             action='store', dest='d',     type=float, default=0.05)
    parser.add_argument('--g',             action='store', dest='g',                 default=1000)
    parser.add_argument('--pInit',         action='store', dest='pInit', type=int)
    
    parser.add_argument('--noDampen',      action='store_true')
    parser.add_argument('--supressOutput', action='store_true')
    parser.add_argument('--writeTxt',      action='store_true')
    parser.add_argument('--plot',          action='store_true')
    
    
    
    # parse and pass arguments into a dictionary
    args = parser.parse_args().__dict__
    
    verbose = not args['supressOutput']
    
    if not isinstance(args['agentType'],list):
        agentTypeList = [args['agentType']]
    else:
        agentTypeList = args['agentType']
    
    if args['allTypes']:
        agentTypeList = ['baselineBidder','straightMV','targetMV','targetMVS']
    
    
        
    if args['pInit'] == None:
        pInit = numpy.ones(args['m'],dtype='float')
    else:
        pInit = numpy.array(args['pInit'],dtype='float')
        
    if verbose:
        print'Computing Symmetric Self Confirming Point Price Prediction.'
        
        print'Agent Type(s)               = {0}'.format(agentTypeList)
        
        print 'Termination Threshold (d)  = {0}'.format(args['d'])
        
        print 'pInit                       = {0}'.format(pInit)
        
        print'Number of Iterations        = {0}'.format(args['L'])
        
        print'Number of Games             = {0}'.format(args['g'])
        
        print'Number of Items per Auction = {0}'.format(args['m'])
        
        print 'Using Dampining             = {0}'.format(args['noDampen'])
        
        print'Number of Parallel Cores    = {0}'.format(multiprocessing.cpu_count())
        
    # Cache local variables that will be needed in loops
    # so we don't have to look up in args dictionary
    # every iteration (swap memory for cpu cycles)
    L = args['L']
    dampen = not args['noDampen']
    g = args['g']    
    delta = args['d']
    
    if not os.path.isdir(args['outDir']):
        os.makedirs(args['outDir'])
        
    
    for agentType in agentTypeList:
        
        if not (agentType != 'baselineBidder' or
                agentType != 'targetMV' or
                agentType != 'targetMVS' or
                agentType != 'straightMV'):
           err = 'Unknown Agent Type {0}'.format(agentType)
           sys.exit(err)
    
        mySymmetricPPP = symmetricPPP(agentType = agentType, m=args['m'])
        
        currentPricePrediction = []
        currentPricePrediction.append(pInit)
        
        dist = numpy.zeros(L,dtype=numpy.float64)
        
        niter = 0
        kappa = 1
        for t in xrange(0,L + 1):
                    
            print ""
            pool = multiprocessing.Pool()
            
            if verbose:
                print 'Iteration: {0}'.format(t)
            
            # set up the dampining constant if so specified
            if dampen:
                kappa = float(L-t+1)/L
                       
                
            if verbose:    
                start=time.clock()
                
            # Run all games in parallel
            # use iterator object to conserve memory, the arguments 
            # are only returned when needed instead of storeing the whole list
            result = numpy.atleast_2d(pool.map(mySymmetricPPP, itertools.repeat(currentPricePrediction[-1],times = g))).astype('float64')
    #        result = pool.map(symmetricPPPtargetMV,itertools.repeat(currentPricePrediction,times = k))
    #        result = pool.map(symmetricPPPtargetMV,[currentPricePrediction]*k)
            
            if verbose:
                finish=time.clock()
                print'Finished {0} Games in {1} seconds.'.format(g,finish-start)
                
            pool.close()
            
            pool.join()
                
            if verbose:
                start=time.clock()
                
            meanP = numpy.mean(result,axis=0,dtype=numpy.float64)
                
            if verbose:
                finish=time.clock()
                print 'Calculated mean in {0} seconds.'.format(finish-start)
                print 'kappa = {0}'.format(kappa)
                print 'Mean Price Vector = {0}'.format(meanP)
                
            # update the point prediction
            currentPricePrediction.append(currentPricePrediction[-1] + kappa*(meanP-currentPricePrediction[-1]))
            
            if verbose:
                print 'New Predicted Price Vector = {0}'.format(currentPricePrediction[-1])
                print 'Previous Price Vector = {0}'.format(currentPricePrediction[-2])
                
                       
            dist[t] = numpy.max(numpy.abs(currentPricePrediction[-1]-currentPricePrediction[-2]))
            
            if verbose:
                    print 'Distance between Successive Iterations = {0}'.format(dist[t])
                    
            nitr = t
                    
            if dist[t] <= delta:
                
                pricePredictionBinFilename = os.path.join(args['outDir'],'pricePrediction_' + agentType + '.npy')
                pricePredictionFile = open(pricePredictionBinFilename,'w')
                numpy.save(pricePredictionFile, currentPricePrediction[-1])
                pricePredictionFile.close()
                
                if args['writeTxt']:
                    pricePredictionTxtFilename = os.path.join(args['outDir'],'pricePrediction_' + agentType + '.txt')
                    pricePredictionTxtFile = open(pricePredictionTxtFilename,'w')
                    numpy.savetxt(pricePredictionTxtFile,currentPricePrediction[-1])
                    pricePredictionTxtFile.close()
                    
                
                if args['plot']:
    #                print 'dist = {0} <= delta = {1} hence terminating'.format(dist,delta)
                    
                    fig = plt.figure()
                    ax = fig.add_subplot(111)
                    x = numpy.arange(0,L,1)
                    ax.plot(x,dist)
                    ax.set_xlabel('Number of Iterations')
                    ax.set_ylabel('Point-wise Price Distance')
                    ax.grid(True)
                    plt.show()
                    
                break;
#                exitStatement='dist[t] = {0} <= delta = {1} hence terminating'.format(dist[t],delta)
#                sys.exit('dist = {0} <= delta = {1} hence terminating'.format(dist[t],delta))
        
                    
        termStatement = 'Terminated after {0} Iterations'.format(nitr)   
        sys.exit(termStatement)

if __name__ == "__main__":
    main()