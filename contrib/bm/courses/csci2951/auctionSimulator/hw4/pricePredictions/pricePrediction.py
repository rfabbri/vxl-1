"""
this is /auctionSimulator/hw4/pricePrediction.py

Author: Brandon Mayer
Date:   11/19/2011

Functions to generate price predictions
"""
from auctionSimulator.hw4.agents.baselineBidder import *
from auctionSimulator.hw4.agents.targetPrice import *
from auctionSimulator.hw4.agents.targetMV import *
from auctionSimulator.hw4.agents.targetMVS import *
from auctionSimulator.hw4.agents.straightMV import *

from auctionSimulator.hw4.auctions import *

import numpy

def symmetricPointPricePredictionWorker(agentType, pointPricePrediction, m = 5 ):
    """
    Helper function for symmetricPointPricePrediction function.
        This function will be sent to each processor in parallel for asyncronious 
        execution of point price prediction.
        
    We instantiate the agents here because each new thread will need a new agent.
    We don't want to have to wait for one process to finish, then reset the agent
    then run the process, we'd rather just create new ones on the fly and run everything
    in parallel
    """
    agent = []
    
    if agentType == 'straightMV':
        agent = straightMV(m=m)
    elif agentType == 'targetPrice':
        agent = targetPrice(m=m)
    elif agentType == 'targetMV':
        agent = targetMV(m=m)
    elif agentType == 'targetMVS':
        agent = targetMVS(m=m)
    else:
        agent = baselineBidder(m=m)
    
    return agent.bid(pointPricePrediction).astype('uint8')

def symmetricPointPricePrediction(agentType="baselineBidder",
                                  m = 5,
                                  L = 100,
                                  priceInit = None,
                                  kappa = True,
                                  delta = 0.05,
                                  k=1000000,):
    """
    Function to replicate a self confirming point price prediction of 
    Yoon & Wellman 2011
    
    Assume homogeneous pool of symmetric agents: All agents play by the same
    strategy profile and draw valuations from the same distribution.
    If this is the case, we can simulate the auction as a single agent bidding
    given a current price prediction.
    
    INPUTS:
        agent := A string specifying the agent type
                   Valid Choices:
                       baselineBidder
                       straightMV
                       targetMV
                       targetMVS
                       targetPrice
                
        m         := number of items in a given auction, default at 5
        
        L         := Maximum number of iterations (default = 100)
        
        kappa     := boolean flag specifying the use of the decaying sequence in 
                     Yoon & Wellman 2011
                     True -> k_t = (L - (t+1))/L
                     False-> k_t = 1
                     
        delta     := a constant less than one specifying the termination condition's
                     threshold between successive price point predicitons.
                     Default = 0.05
        
        priceMin  := Minimum price that can occur in an auction for any good
                     default = 1
        
        priceMax  := Maximum price that can occur in the auction for any good
                     default = 50
                     
        k         := Number of games to run per iteration
    """
    
    if not priceInit:
        priceInit = numpy.ones(m)
        
    for t in xrange(0,L+1):
        
        if not kappa:
            k = (L-t+1)/L
        else:
            k = 1
            
        # because we assumpe  0 <= price <= 50 we can get away with an unsigned byte
        # which has range 0->127
        gameResults = numpy.zeros((k,m),dtype='uint8')
        
        for game in xrange(0,k):
           
            
            
            
    