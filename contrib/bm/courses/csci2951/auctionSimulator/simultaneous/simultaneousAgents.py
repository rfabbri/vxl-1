'''
This is /auctionSimulator/simultaneous/agents.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file containing agent specialization for simultaneous N auction
setting.

'''
from auctionSimulator import base
from auctionSimulator.valuation import *
import math

class pureRandomBidSim(object):
    """Will bid a random value from 0 to 1 dispite valuation"""
    def __init__(self):
        self.name="pureRandomBid"
    def __call__(self,v):
        import random
        return random.uniform(0,1)
    
class randomBidSim(object):
    """Will bid a random value distributed from floor to valuation"""
    def __init__(self,floor=0):
        self.name = "randomBid"
        self.floor=floor
    def __call__(self, v):
        import random
        return random.uniform(self.floor,v)

class constantBidSim(object):
    def __init__(self,c=.5):
        self.name = "constantBid"
        self.c = c
    def __call__(self,v):
        return self.c
        
class smartConstBidSim(object):
    def __init__(self,c=.5):
        self.name="smartConstBid"
        self.c = c
    def __call__(self,v):
        if self.c <= v:
            #print'smartConstBid bidding c={0}'.format(self.c)
            return self.c
        else:
            #print'smartConstBid bidding v={0}'.format(v)
            return v
        
class simNAgent(base.agentBase):
    def __init__(self,name="anonymousSimNAgent", vObject = randomValuation(), bObject=randomBidSim() ):
        self.name=name
        super(simNAgent, self).__init__(name)
        
        # function objects describing the bid and valuation function
        # for agent instance
        self.vObject=vObject
        self.bObject=bObject
                 
    def printValuationObjectName(self):
        print self.vObject.name
        
    def printBidObjectName(self):
        print self.bObject.name
            
    def printProfit(self):
        if( self.won == None or self.final_price== None ):
            print 'Unknown Auction Outcome'
        elif( len(self.won) != len(self.final_price) ):
            print'----ERROR---- len(self.wins)\
                        != len(self.final_price)'
        else:
            profit = 0
            for i in self.won:
                if not math.isnan(self.final_price[i]):
                    profit += self.v[i] - self.won[i]*self.final_price[i] 
                
    def printValuation(self):
        if self.v == None:
            print'{0} valuation is undetermined as yet.'.format(self.name)
        else:
            print'{0:10} {1:10} {2}'.format(self.name,"valuation:",self.v)
            
    def printBids(self):
        if self.b == None:
            print'{0}\'s bids are undetermined as of yet.'.format(self.name)
        else:
            print'{0:10} {1:10} {2}'.format(self.name,"bids:",self.b)
            
    def printFinalPrice(self):
        if self.final_price == None:
            print'{0]\'s final prices are undetermined as of yet.'.format(self.name)
        else:
            print'{0:10} {1:10} {2}'.format(self.name,"final prices:",self.final_price)
            
        
    def numAuctionsWon(self):
        sum = 0;
        for i in range(len(self.won)):
            if not math.isnan(self.won[i]):
                sum+=self.won[i]
        return sum
        
    def revenueTotal(self):
        """Return the value of items won"""
        revenue = 0;
        if len(self.won) == len(self.final_price) and \
            len(self.won) == len(self.v):
            revenue = 0;
            for i in range(len(self.won)):
                revenue += self.won[i]*self.v[i]
            
            return revenue
        else:
            print '----ERROR----'
            print '\t Won bit vector and final price vector an incompatible.'
            
    def costTotal(self):
        """Return the total cost of purchasing the items won."""
        if len(self.won) == len(self.final_price):
            cost=0
            for i in range(len(self.won)):
                if not math.isnan(self.final_price[i]):
                    cost+=self.won[i]*self.final_price[i]
            return cost
        else:            
            print '----ERROR----'
            print '\t Won bit vector and final price vector an incompatible.'
   
    def profitTotal(self):
        """Return the total profit from the auction"""
        if len(self.won)==len(self.v):
            cost = self.costTotal()
            revenue = self.revenueTotal()
            return revenue - cost
        else:
            print '----ERROR----'
            print '\t Won bit vector and value vector are incompatible.'
            
    def printAuctionSummary(self):
        """Print a Summary of Auction Results to std out"""
        print' '
        print'------Auction Summary for {0}------'.format(self.name)
        
        #print'\t{0:20} {1}'.format("Final Prices:",self.final_price)
        #print'\t{0:20} {1}'.format("Items Won:",self.won)
        print'\t{0:20} {1}'.format("Number Items Won:",self.numAuctionsWon())
        #print'\t{0:20} {1}'.format("Valuations:",self.v)
        print'\t{0:20} {1}'.format("Total Revenue: ",self.revenueTotal())
        print'\t{0:20} {1}'.format("Total Cost:", self.costTotal())
        print'\t{0:20} {1}'.format("Total Profit:",self.profitTotal())
        print' '
   
    def bid(self,k):
        """create a bid or a vector of bids on k items"""
        self.v=[]
        self.b=[]
        for i in range(k):
            #1. get item valuation
            self.v.append(self.vObject())
            #2. draw a bid
            #print'self.v[-1] = {0}'.format(self.v[-1])
            self.b.append(self.bObject(self.v[-1]))            
        return self.b 
    

class oneShotAgent(base.agentBase):
    def __init__(self,name="anonymousOneShotAgent", vObject = randomValuation(), bObject = randomBidSim() ):
        self.name=name
        super(oneShotAgent, self).__init__(name)
        
        # function objects describing the bid and valuation function
        # for agent instance
        self.vObject=vObject
        self.bObject=bObject   
        
    def cost(self):
        if not self.final_price == None:
            return self.final_price*self.won
        else:
            print '----ERROR----'
            print '\t oneShotAgent.cost(): self.v or self.final_price = None'
            return 0
       
    def revenue(self):
        if not self.won == None and\
                not self.v == None:
            return self.won*self.v
        else:
            print '----ERROR----'
            print '\t oneShotAgent.revenue(): self.won or self.v = None'
            return 0
        
    def profit(self):
        return self.revenue() - self.cost()
    
    def printBalanceSheet(self):
        """Print a balance sheet to std out"""
        print' '
        print'------Auction Summary for {0}------'.format(self.name)
        print'\t{0:20} {1}'.format("Valuation:",self.v)
        print'\t{0:20} {1}'.format("Bid:",self.b)
        print'\t{0:20} {1}'.format("Revenue:",self.revenue())
        print'\t{0:20} {1}'.format("Cost:", self.cost())
        print'\t{0:20} {1}'.format("Profit:",self.profit())
        print' '
        
        
    def bid(self):
        """Bid on the single item"""
        self.v = self.vObject()
        self.b = self.bObject(self.v)       
        return self.b


          