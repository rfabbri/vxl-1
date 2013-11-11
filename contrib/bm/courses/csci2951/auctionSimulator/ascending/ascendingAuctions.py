'''
This is /auctionSimulator/ascending/ascendingAuctions.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file classes that run ascending auctions.


'''
from auctionSimulator import base
import numpy
import heapq
import math #for math.isnan
    
class ascendingAuction(base.auctionBase):
    """"A class for simulating an Ascending Auction
        Inputs: 
            agents:               list of participating agents
            num_bids_thresh:      threshold on the number of auction rounds
            reserve:              starting ask price
            N:                    specifies the Nth pay rule
            bidIncrement          ask = max(bid) + bidIncrement
            """
    
    def __init__(self,name="Ascending Auction",  agents=[], N = 1, num_bids_thresh=1000, reserve = 0.0001, bidIncrement = 0.01, verbose = True):
        #call the base constructor
        super( ascendingAuction, self).__init__(name,agents)
        
        self.num_bids_thresh = num_bids_thresh
        
        self.reserve=reserve
        
        self.bidIncrement = bidIncrement
        
        # More general than just 1st price or second price
        # The winner will pay the last price as long as the reserve is met
        # else he/she will pay the reserve
        if len(self.agent_list) < N:
            self.n = len(self.agent_list)
        else:
            self.n = N
            
        self.verbose = verbose
        
    
                                
    def printAuctionResults(self):
        if self.auction_run == False:
            print 'Auction has not been run yet.'
        else:
            print ' '
            print '----------------Auction Summary---------------------'
            print'Ascending Auction, N = {0} pay rule, {1} reserve.'.\
                format(self.n,self.reserve)
            if self.winners:
                #print'\t{0:15} {1}'.format('Ask History:',self.ask)
                print'\t{0:15} {1}'.format('Final Price:',self.final_price)
                print'\t{0:15} {1}'.format('Winner:', self.agent_list[numpy.argmax(self.winners)].name)
                print'\t{0:15} {1}'.format('Number of Iterations', self.itr)
            else:
                print'Reserve was not met, the auction has no winners.'
                
    def printBidHistory(self):
        if self.auction_run:
            for agent in self.agent_list:
                agent.printBidHistory()
        else:
            print '----ERROR ascendingAuctions.printBidHistory----'
            print '\tAuction not run yet.'
            
    def printAskHistory(self):
        if self.auction_run:
            print'Ask History: {1}'.format(self.ask)
            
        else:
            print '----ERROR ascendingAuctions.printAskHistory----'
            print '\tAuction not run yet.'

    def printBalanceSheets(self):
        if self.auction_run:
            for agent in self.agent_list:
                agent.printBalanceSheet()
        else:
            print'Auction Not Run Yet.'
            
            
    def runAuction(self):
        """Main function to simulate the auction."""
        #initialize with base class function
        super( ascendingAuction, self).runAuction()
        
        if not self.agent_list:
            print '----ERROR----'
            print'{0:10} has no agents attached'.format(self.name)
            return
        
        self.itr = 0
        done = False
        while not done and self.itr <= self.num_bids_thresh:
            bids = []
            
            if self.itr == 0:
                print '----Initializing Auction {0} -----'.format(self.name)
                
                #initialize the auction ask price
                self.ask = [self.reserve]
                
                # assign an id to each agent who is participating
                for i in range(len(self.agent_list)):
                    self.agent_list[i].id=i
        
                #just for education
                self.highest_bid_history = [self.reserve]               
                
                self.current_price_history = [self.reserve]
                
                self.winner_history = []
        
                #variable from base class
                #will be a bit vector in this specialization
                self.winners = []
        
                #defined on the base class
                #flags if runAuction has been called
                self.auction_run = True
                
                # There are no winners as of yet
                winner = None
                
                # collect the initial bids
                for agent in self.agent_list:
                    bids.append(agent.bid(self.ask[-1],winner))
                         
            else:          
                #collect bids for each agent given the winner previous winner
                for agent in self.agent_list:
                    bids.append(agent.bid(self.ask[-1],self.winner_history[-1]))
                
            w = numpy.argmax(bids)
            
            if bids[w] < self.ask[-1]:
                # No valid bids submitted, the auction is over
                done = True;
                
                #winners is a bit vector indicating the victor
                # if winners == none, then no valid bids were submitted on 
                # the first round
                self.winners = [0]*len(self.agent_list)
                 
                # Check the reserve was met
                if not self.itr == 0:
                    self.winners[self.winner_history[-1]] = 1
                    if(self.current_price_history):
                        self.final_price = self.current_price_history[-1]
                    else:
                        self.final_price=self.reserve      
                else:
                    self.winners = []
                    self.final_price = float('nan')
                            
            else:
                # Increment the ask update winner and price and keep going
                self.ask.append(bids[w] + self.bidIncrement)
                self.winner_history.append(w)
                self.highest_bid_history.append(bids[w])
                if len(self.highest_bid_history) > self.n:
                    self.current_price_history.append(heapq.nlargest(self.n,self.highest_bid_history)[-1])
                else:
                    self.current_price_history.append(self.reserve)

            if self.winner_history:    
                if not done:
                    print ' '            
                    print 'Iteration: {0}'.format(self.itr)
                
                    print '\t{0:15} {1}'.\
                        format('Current Winner: ',self.agent_list[self.winner_history[-1]].name)
                    print '\t{0:15} {1}'.\
                        format('Current Price: ',self.current_price_history[-1])
                    print '\t{0:15} {1}'.\
                        format('Current Ask: ', self.ask[-1])
                else:
                    print' '
                    print 'Auction Has Ended'
                    print '\t{0:15} {1}'.\
                        format('Winner: ',self.agent_list[self.winner_history[-1]].name)
                    print '\t{0:15} {1}'.\
                        format('Price: ',self.current_price_history[-1])   
            else:
                print'Auction reserve {0} not met. Item not allocated.'.format(self.reserve)
                
                             
            self.itr += 1
        
        #notify agents of results
        if self.winners:
            self.final_price=self.current_price_history[-1]
            for i in range(len(self.winners)):
                #notify all of final price
                self.agent_list[i].final_price=self.final_price
               
                if self.winners[i] == 1:
                    self.agent_list[i].won = 1    
                else:
                    self.agent_list[i].won = 0
        else:
            #there were no winners
            for agent in self.agent_list:
                agent.won = 0
                agent.final_price = 0   
        
        if self.verbose:
            self.printBalanceSheets()         
                