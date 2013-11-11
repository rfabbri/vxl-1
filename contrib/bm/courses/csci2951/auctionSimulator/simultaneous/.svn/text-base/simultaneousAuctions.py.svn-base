from auctionSimulator.simultaneous import simultaneousAgents 
from auctionSimulator import base
import random # for random.shuffle
import numpy
import heapq
import math
from pylab import*

class simNAuction(base.auctionBase):
    """A class representing a simultaneous N'th pay auction"""
    
    def __init__(self,name="Simultaneous Auction",k=1,r=.001,n=1,agents=[]):    
        #call base constructor
        super( simNAuction, self).__init__(name,agents)
        
        # Number of auctions
        self.k=k
        
        # Reserve Price for Auctions
        self.r=r
        
        # Nth payment rule
        self.n=n
                     
    def runAuction(self):
        self.bids = []
        #1. have the agents bid
        for i in range(len(self.agent_list)):
            self.bids.append([])
            self.bids[i] = self.agent_list[i].bid(self.k)
        
        
        #2. Determine highest bid and nth price
        # if n < number of agents, then the payment price
        # is reserve
        self.winners = []
        self.final_price = []
        
        # numpy.argmax(2darray,0) returns the argmax over
        # columns
        w = numpy.argmax(self.bids,0)
        
        #check if the reserves are met
        # reserve_met = []
        # for j in range(self.k):
            # if self.bids[w[j]][j] > self.r:
                # reserve_met = 1
            # else:
                # reserve_met = 0
                
        
        # determine final prices for items where the reserve is met
        if len(self.agent_list) < self.n:
            for j in range(self.k):
                self.final_price.append(self.r)
        else:
            for j in range(self.k):
                auction_prices = []
                for i in range(len(self.agent_list)):
                    auction_prices.append(self.bids[i][j])
                
                if self.bids[w[j]][j] < self.r:
                    #print 'The Winning Bid did not beat reserve'
                    self.final_price.append((float('nan')))
                else:
                    # returns the nth heighest price
                    n_price = heapq.nlargest(self.n,auction_prices)[-1]
                    # check that the nth price is above the reserve
                    if n_price < self.r:
                        self.final_price.append(self.r)
                    else:
                        self.final_price.append(n_price)
                    
                
                
        
        
        
        # turn index denominated representation to bit matrix
        self.winners=[]

        for i in range(len(self.agent_list)):
            self.winners.append([0]*self.k)
            for j in range(self.k):
                if w[j] == i and not math.isnan(self.final_price[j]):
                    self.winners[i][j]=1

                    
            # give the agent the bit vector of his/her wins
            self.agent_list[i].won=self.winners[i]
                
            # notify all agents of final prices
            for agent in self.agent_list:
                agent.final_price = self.final_price
                
        
                       
        self.auctionRun=True
        
        self.printAuctionResults()
        

    def printWinners(self):
        # Print a list of the auction number, the name of the winner, and 
        # the price he/she must pay
        if self.auctionRun == True:
            #for i in range(len(self.agent_list)):
            for j in range(self.k):
                has_winner = False
                #for j in range(self.k):
                for i in range(len(self.agent_list)):
                    if self.winners[i][j] == 1:
                        has_winner = True
                        print'Auction {0} winner is {1:20} and pays: {2}'\
                            .format(j,self.agent_list[i].name,self.final_price[j])
                if has_winner == False:
                    print'Auction {0} has no winner, reserve {1} not met.'.format(j,self.r)
        else:
            print 'Auction has not run yet.'
                
    def printAuctionResults(self):
        if self.auctionRun == False:
            print 'Auction Has not been run yet.'
        else:
            print'{0} Simultaneous One Shot auctions.'.format(self.k)
            print'N={0} pay rule.'.format(self.n)
            print'{0} reserve.'.format(self.r)
            print'{0:10} {1}'.format("Final Prices:", self.final_price)
            for agent in self.agent_list:
                agent.printBids()
                      
            for j in range(self.k):
                has_winner = False
                for i in range(len(self.agent_list)):
                    if self.winners[i][j] == 1:
                        has_winner = True
                        print 'Auction {0} winner is {1:15} and pays: {2}'\
                             .format(j,self.agent_list[i].name,self.final_price[j])
                if has_winner == False:
                    print'Auction {0} has no winner, reverve {1} not met.'.format(j,self.r) 
                
                
    def graphPercentWon(self):    
        """Graph a Pie Chart of Percentage of Auctions Agents have Won"""
        if self.auctionRun == True:
            figure(1, figsize=(6,6))
            ax = axes([0.1, 0.1, 0.8, 0.8])
            
            labels = []
            fracs = []
            explode = []
            for agent in self.agent_list:
                labels.append(agent.name)
                fracs.append(agent.numAuctionsWon())
                explode.append(0.05)
            
            #print'fracs = {0}'.format(fracs)
            
            pie(fracs,explode=explode,labels=labels,autopct='%1.1f%%',shadow=True)
            title('Percentage of Auctions Won by each Agent',
               bbox={'facecolor':'0.8', 'pad':5})
            
            show()  
        else:
            print'Cannot Graph Percent Won: Auction not yet Run.'
       
    def graphAgentProfit(self):
        """Graph a Bar Chart of Agent Profit"""
        if self.auctionRun == True:
            fig = figure()
            ax = fig.add_subplot(111)
            
            profits = []
            agent_names = []
            for agent in self.agent_list:
                agent_names.append(agent.name)
                profits.append(agent.profitTotal())
                
            # the x locations for the groups    
            ind = numpy.arange(len(agent_names))    
            
            # width of the bars
            width = .35    
            
            # the bars
            rects1 = ax.bar(ind, profits,width,color='r')
            
            title_string='Agent Profit:\n {0} auctions {1} reserve {2} N pay rule'.format(self.k,self.r,self.n)
            
            ax.set_ylabel('Profit')
            #ax.set_title('Agent Profit')
            ax.set_title(title_string)
            ax.set_xticks(ind + (width/2))
            ax.set_xticklabels(agent_names)
                
            plt.show()
        else:
            print'Cannot Graph Agent Profit: Auction not yet Run.'
        
class oneShotAuction(base.auctionBase):   
    """
    A class for simulating a Sealed Bid N'th Price One Shot Auction
    A specialization of simNAuction
    """         
    def __init__(self,name="One Shot Auction", agents=[], nprice = 1,reserve = 0.001,verbose = True): 
        #call base constructor
        super( oneShotAuction, self).__init__(name,agents)
        
        #Reserve price
        self.reserve = reserve
        
        #nth pay rule
        self.nprice = nprice
        
        #Verbose flag for self.runAuction text
        self.verbose = True

    def runAuction(self):
        #Initialize
        self.bids = []
        self.winners = []
        self.final_price = []
        
        #1. Collect Bids
        for agent in self.agent_list:
            self.bids.append(agent.bid())
            
        #2. Determine Highest Bidder and Nth Price
    
        #determine the winner and break ties by picking randomly
        w = numpy.where(self.bids==numpy.max(self.bids))[0]
        import random
        random.shuffle(w)
        w=w[0] 
        
        self.winners = [0]*len(self.agent_list)
        
        self.reserveMet = False
        #check if reserve is met
        # if there is no winner, then then
        # self.winners is all zero
        if self.bids[w] > self.reserve:
            self.winners[w] = 1
            self.reserveMet = True
    
        # If the nth price is less than the reserve,
        # winner pays the reserve else the nth price
        if len(self.agent_list) > self.nprice:
            self.final_price = heapq.nlargest(self.nprice,self.bids)[-1]
            
            if self.final_price < self.reserve:
                self.final_price = self.reserve
        else:
            self.final_price = self.reserve
            
        #notify all agents
        for i in range(len(self.agent_list)):
            if self.winners[i] == 1:
                self.agent_list[i].won = 1
            else:
                self.agent_list[i].won = 0
            
            self.agent_list[i].final_price = self.final_price
        
        self.auction_run = True
        
        if self.verbose:
            self.printAuctionSummary()
            self.printBalanceSheets()
        
    def printAuctionSummary(self):
        if self.auction_run:
            w = numpy.where(self.bids==numpy.max(self.bids))[0]
            w = w[0]
            if self.reserveMet:    
                print ' '
                print'-------------{0}-----------------'.format(self.name)
                
                print'{0:15} {1}'.format('Reserve:',self.reserve)
                print'{0:15} {1}'.format('Winner:',self.agent_list[w].name)
                print'{0:15} {1}'.format('Bid: ',self.bids[w])
                print'{0:15} {1}'.format('Final Price:',self.final_price)
            
                
            else:
                print ' '
                print'-------------{0}-----------------'.format(self.name)
                print'There were no winner. Reserve {0} Not Met'.format(self.reserve)
                print'{0:15} {1}'.format('Highest Bid:', self.bids[w])
        else:
            print '-------------{0}-----------------'.format(self.name)
            print'       Has Not Yet Run'
            
    def printBalanceSheets(self):
        if self.auction_run:
            for agent in self.agent_list:
                agent.printBalanceSheet()
        else:
            print' '
            print'----ERROR----'
            print'Cannot Print Balance Sheets, Auction Not Run.'
            
            
        
        
            
            