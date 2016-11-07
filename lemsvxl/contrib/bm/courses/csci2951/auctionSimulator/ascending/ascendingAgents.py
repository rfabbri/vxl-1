'''
This is /auctionSimulator/ascending/ascendingAgents.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file classes for simulating an agent to participate in an 
ascending auction

Must provide each agent with a valuation and bid functor 

'''
from auctionSimulator import base
from auctionSimulator.valuation import *
import math #for math.isnan

class randomBidAscending(object):
    """A functor for bidding a random value from 0 to v. 
       Will bid 0 if cost is greater than valuation. Will bid 0
       if bidder indicates he was the previous winner"""
    def __init__(self,floor=0):
        self.floor = floor
        self.name="randomBidAscending"
    def __call__(self,v,ask,win):
        """
        v:       valuation
        ask:     current ask price
        win:     binary flag"""
        
        if v > ask and v > self.floor and not win:
            import random
            return random.uniform(self.floor,v)
        else:
            return 0

class randomValidBidAscending(object):
    """A functor for bidding a random value from 0 to v. 
       Will bid 0 if cost is greater than valuation. Will bid 0
       if bidder indicates he/she was the previous winner"""
    def __init__(self):
        self.name = "randomValidBidAscending"
    def __call__(self,v,ask,win):
        """
        v:       valuation
        ask:     current ask price
        win:     binary flag"""
        if v > ask and not win:
            import random
            return random.uniform(ask,v)
        else:
            return 0
        
class smartBidAskAscending(object):
    """A functor that will bid the ask price if the agent isn't already winning up to the valuation. Will
    bid zero otherwise."""
    def __init__(self):
        self.name = "bidAskAscending"
    def __call__(self,v,ask,win):
        """
        v:       valuation
        ask:     current ask price
        win:     binary flag
        """
        if v > ask and not win:
            return ask
        else:
            return 0
           
class bidAskAscending(object):
    """A functor that will bid the ask price if the agent isn't already winning.
    Valuation is not taken into consideration can (and probabily will) have negative return. Will
    bid zero otherwise."""
    def __init__(self):
        self.name = "bidAskAscending"
    def __call__(self,v,ask,win):
        """
        v:       valuation
        ask:     current ask price
        win:     binary flag
        """
        if not win:
            return ask
        else:
            return 0
    
class ascendingAgent(base.agentBase):
    """A class to simulate agents in ascending auctions. We assume that there is a single
    item in the ascending auction so we can obtain the valuation in the constructor"""
    def __init__(self,name="anonymous", vObject = randomValuation(), bObject=randomBidAscending() ):
        # This will call the base agent constructor which  
        super(ascendingAgent, self).__init__(name)
        
        self.vObject = vObject
        self.bObject = bObject
        
        #valuation is determined at initialization, can go right
        # to bidding
        self.v = vObject()
        
        # unnecessary but for education
        self.bid_history = []
        
    def initAgent(self):
        # This will call the base agent constructor which  
        super(ascendingAgent, self).initAgent()
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
                
    def bid(self,ask,winner):
        """Winner is the current winner of the auction"""
        if winner == self.id:
            win = 1
        else:
            win = 0
        self.bid_history.append(self.bObject(v=self.v,ask=ask,win=win))
        return self.bid_history[-1]
       
    
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
            
            
        
            