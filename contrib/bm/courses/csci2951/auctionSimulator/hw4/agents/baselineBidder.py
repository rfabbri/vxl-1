"""
this is /auctionSimulator/hw4/baselineBidder.py

Author: Brandon Mayer
Date: 11/17/2011

Specialized agent class to replicate baselineBidding from
Yoon and Wellman (2011)
"""
from agentBase import *

class baselineBidder(agent):
    """
    Replicated BaselineBidding agent from Yoon & Wellman 2011
    """
    def __init__(self, m = 5,
                 v_min = 1, 
                 v_max = 50,
                 name = "Anonymous baselineBidder"): 
        super(baselineBidder,self).__init__(m,v_min,v_max,name)
        
    def type(self):
        return "baselineBidder" 
        
    def bid(self,args={}):
        """
        "BaselineBidding agents with a 
        single-unit task bid true value for the first slot, and otherwise they bid
        for the first \lambda_i goods at price v_i^(\labmda_i)/(\lambda_i)" 
        (Yoon & Wellman, 2011)
        
        NOTE: 
            Base line bidder does not solve for a target bundle to bid (acq) 
            it just bids for the first self.l items at a price of excess value
            distributed normally over the set of goods
        """
        bid = []
        
        for i in range(self.m):
            if i < self.l:
                bid.append(float(self.v[self.l-1])/self.l)
            else:
                bid.append(0)
                      
        return numpy.array(bid)   