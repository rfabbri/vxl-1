'''
This is /auctionSimulator/descending/descendingAgents.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file classes for simulating an agent to participate in a 
descending auction

Must provide each agent with a valuation and bid functor 

'''
from auctionSimulator import base
from auctionSimulator.valuation import *
import math #for math.isnan

class geometricBidDescending(object):
    """ A functor for bidding via a geometric progression
    once the ask falls below the agent's valuation
    """
    def __init__(self,p=0.5):
        self.name = "geometricBidDescending"
        self.p = p
    def __call__(self,v,ask):
        """
        v:         valuation
        ask:       current ask price
        """
        if v > ask:
            import random
            r = random.uniform(0,1)
            if r < self.p:
                return ask
            else:
                return 0
        else:
            return 0
        
class valuationBidDescending(object):
    """
    A functor that bids the valuation once it is reached.
    """        
    def __init__(self):
        self.name="valuationBidDescending"
        
    def __call__(self,v,ask):
        """
        v:        valuation
        ask:      current ask price
        """
        if v >= ask:
            return ask
        else:
            return 0
        
        
class vLessConstDescending(object):
    """
    Bid a constant value below the valuation
    """        
    def __init__(self,c):
        self.name="vLessConstDescending"
        self.c=c
    def __call__(self,v,ask):
        if v-self.c >= ask:
            return ask
        else:
            return 0
        
class descendingAgent(base.agentBase):
    """
    A class to simulate agents in descending auctions.
    """
    def __init__(self,name="anonymousDescendingAgent", vObject = randomValuation(), bObject=geometricBidDescending() ):
        super(descendingAgent, self).__init__(name)
        
        self.vObject = vObject
        self.bObject = bObject
        
        #valuation is determined at initialization, can go right
        # to bidding
        self.v = vObject()
        
        # unnecessary but for education
        self.bid_history = []
        
    def initAgent(self):
        super(descendingAgent, self).initAgent()
        self.v=self.vObject()
        
    def printValuation(self):
        if self.v == None:
            print '{0} valuation is undetermined as yet.'.format(self.name)
        else:
            print '{0:10} {1:10} {2}'.format(self.name,"valuation",self.v)
            
    def printBidHistory(self):
        if not self.bid_history:
            print '{0} has not place any bids yet.'.format(self.name)
        else:
            print '{0:10} {1:10} {2}'.format(self.name,"Bid History:",self.bid_history)
            
    def profit(self):
        return self.revenue()-self.cost()
        
    def printProfit(self):
        profit = self.profit()
        if profit:
            print '{0:10} {1:10} {2}'.format(self.name,"profit:",profit)
        else:
            print '{0:10} Could not calculate profit'.format(self.name)
            
    def printCost(self):
        cost = self.cost()
        if cost:
            print '{0:10} {1:10} {2}'.format(self.name,"cost:",cost)
        else:
            print '{0:10} Could not calculate cost'.format(self.name)
            
            
    def printBalanceSheet(self):
        """Print a Summary of Auction Results to std out"""
        print ' '
        print'------Balance Sheet for {0}------'.format(self.name)
        #print'\t{0:20} {1}'.format("First 20 Bids: ",self.bid_history[0:20])
        
        if self.won:
            print'\t{0:20} {1}'.format("Won:","True")
        else:
            print'\t{0:20} {1}'.format("Won:","False")
        
        print '\t{0:20} {1}'.format("Valuation:",self.v)
        print'\t{0:20} {1}'.format("Total Revenue: ",self.revenue())
        print'\t{0:20} {1}'.format("Total Cost:", self.cost())
        print'\t{0:20} {1}'.format("Total Profit:",self.profit())
        print' '
        
    def cost(self):
        if not math.isnan(self.final_price):
            return self.final_price*self.won
        else:
            return 0
            
    def revenue(self):
        if not math.isnan(self.final_price):
            return self.v*self.won
        else:
            return 0
           
    def profit(self):
        return self.revenue()-self.cost()
        
    def bid(self,ask):
        """"
        Wrapper for bidding in descending auctions.
        """
        self.bid_history.append(self.bObject(v=self.v,ask=ask))
        return self.bid_history[-1]
    
                
        