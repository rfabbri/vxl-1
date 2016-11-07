'''
This is /auctionSimulator/sequential/sequentialAgents.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file classes for simulating an agent to participate in a 
sequential auction

Must provide each agent with a valuation and bid functor 

'''
from auctionSimulator import base
from auctionSimulator.valuation import *
from auctionSimulator.ascending.ascendingAgents import *
from auctionSimulator.descending.descendingAgents import *
from auctionSimulator.simultaneous.simultaneousAgents import *

class sequentialAgent(base.agentBase):
    """
    A class to simulate agents in descending auctions.
    INPUTS:
    agent_dictionary:    a dictionary of string types to agents
    """
    def __init__(self,
                 name="annonymousSequentialAgent",
                 agent_dictionary = {} ):
        
        super(sequentialAgent,self).__init__(name)
        
        self.agent_dictionary = agent_dictionary
        
        self.auctionSchedule = []
        
    def initAgent(self):
        """
        For the auction class to call at the beggining of runAuction
        """
        #call the base class function
        super(sequentialAgent,self).initAgent()
        #clear the previous schedule
        self.auctionSchedule = []
        #the underlying agents will be intialized in the auction
        #in which they participate
        
        
    def addAgent(self,type,agent):
        """
        Will replace the old agent with the new one if the entry already
        exists.
        """
        self.agent_dictionary[type] = agent
        
    def getAgent(self,type):
        """
        Returns the agent to the auction
        """
        if type in self.agent_dictionary:
            return self.agent_dictionary[type]
        else:
            return none
        
    def numAuctionsWon(self):
        sum = 0
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
            
    def getValuations(self):
        self.v=[]
        if self.auctionSchedule:
            for k,val in self.agent_dictionary.iteritems():
                if isinstance(val.v,list):
                    self.v.append(val.v[0])
                else:
                    self.v.append(val.v)
        else:
            print 'No Auction Schedule Specified'
                    
    def printBalanceSheet(self):
        #self.getValuations()
        """Print a Summary of Auction Results to std out"""
        print' '
        print'------Auction Summary for {0}------'.format(self.name)
        
        #print'\t{0:20} {1}'.format("Final Prices:",self.final_price)
        #print'\t{0:20} {1}'.format("Items Won:",self.won)
        #print'\t{0:20} {1}'.format("Number Items Won:",self.numAuctionsWon())
        print'\t{0:20} {1}'.format("Valuations:",self.v)
        print'\t{0:20} {1}'.format("Won: ",self.won)    
        print'\t{0:20} {1}'.format("Final Prices: ",self.final_price)
        print'\t{0:20} {1}'.format("Total Revenue: ",self.revenueTotal())
        print'\t{0:20} {1}'.format("Total Cost:", self.costTotal())
        print'\t{0:20} {1}'.format("Total Profit:",self.profitTotal())
        print' '
        
    

        
        