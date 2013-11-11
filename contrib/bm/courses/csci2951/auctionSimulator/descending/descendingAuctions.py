'''
This is /auctionSimulator/descending/descendingAuctions.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file classes for simulating a descending auction
 
'''
from auctionSimulator import base
import numpy #for numpy.argmax
import heapq
import math #for math.isnan
import random #for random.shuffle

class descendingAuction(base.auctionBase):
    """A class for simulating a descending auction
        Inputs: 
            agents:               list of participating agents
            num_itr_thresh:      threshold on the number of auction rounds
            reserve:              starting ask price
            decrement             ask(t) = ask(t-1) - decrement
    """

#rename reserve price
    def __init__(self,name="Descending Auction", agents = [],num_itr_thresh=1000,reserve=1.0,decrement = 0.01):
        #call the base constructor
        super( descendingAuction, self).__init__(name,agents)
        
        self.num_itr_thresh = num_itr_thresh
        
        self.reserve=reserve
        
        self.auction_run = False
        
        self.decrement = decrement
        
    def printBalanceSheets(self):
        if self.auction_run:
            for agent in self.agent_list:
                agent.printBalanceSheet()
        else:
            print'Auction Not Run Yet.'
            
    def runAuction(self):
        """Main function to simulate the auction."""
        #initialize with parent class
        super( descendingAuction, self).runAuction()
        
        if not self.agent_list:
            print '----ERROR----'
            print'{0:10} has no agents attached'.format(self.name)
            return
        
        #variable on base class
        self.auction_run = True
        
        print '----Initializing Auction {0} -----'.format(self.name)
                
        #initialize the auction ask price
        self.ask = [self.reserve]
            
        # assign an id to each agent who is participating
        for i in range(len(self.agent_list)):
            self.agent_list[i].id=i
                    
        #clear variable on base class
        self.winners = []
        
        self.itr = 0
        done = False
        
        while not done\
            and self.itr <= self.num_itr_thresh\
                and self.ask[-1]-self.decrement > 0:
            
            print 'Iteration: {0}'.format(self.itr)
            print '\t{0:15} {1}'.\
                format('Current Ask: ', self.ask[-1])
                
            bids = []
                
            #collect bids
            for agent in self.agent_list:
                bids.append(agent.bid(self.ask[-1]))
              
            #break ties by choosing the winner randomly
            w = numpy.where(bids==numpy.max(bids))[0]
            random.shuffle(w)
            w=w[0]
            
            
            if bids[w] >= self.ask[-1]:
                done = True
                
                self.winners = [0]*len(self.agent_list)
                self.winners[w] = 1
                
                self.final_price=bids[w]
            else:
                #update ask and keep going
                self.ask.append(self.ask[-1]-self.decrement)        
                self.itr += 1
                
        #auction clean up/allocation
        if self.winners:
            print' '
            print 'Auction Has Ended'
            print '\t{0:15} {0}'.format('Number of Iterations',self.itr)
            print '\t{0:15} {1}'.\
                        format('Winner: ', self.agent_list[numpy.argmax(self.winners)].name)
            print '\t{0:15} {1}'.\
                        format('Price: ', self.final_price)
            
            #there was a winner
            for i in range(len(self.winners)):
                self.agent_list[i].final_price=self.final_price
                
                if self.winners[i] == 1:
                    self.agent_list[i].won = 1
                else:
                    self.agent_list[i].won = 0
        else:
            #there was no winner
            for agent in self.agent_list:
                agent.won = 0
                agent.final_price = 0
                
            
        
                
                  
                
                
                
        
        