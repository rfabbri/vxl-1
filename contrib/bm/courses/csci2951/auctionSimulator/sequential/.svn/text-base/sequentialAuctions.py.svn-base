'''
This is /auctionSimulator/sequential/sequentialAuctions.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file classes for simulating a sequential auction
 
'''
from auctionSimulator import base
from auctionSimulator.valuation import *
from auctionSimulator.ascending.ascendingAuctions import ascendingAuction
from auctionSimulator.descending.descendingAuctions import descendingAuction
from auctionSimulator.simultaneous.simultaneousAuctions import *

class sequentialAuctions(base.auctionBase):
    """A class for simulating a descending auction
        Inputs: 
            agents:               list of participating agents
            auction_parameters    list of list of auction parameters
                                  the rows corresponds to auction numbers
                                  the rows the appropriate parameters for the
                                  auction type
    """
    def __init__(self,name="Sequential Auction", agents=[], auction_parameters=[]):
        super(sequentialAuctions,self).__init__(name,agents)
        
        self.auction_parameters = auction_parameters
        
        self.auction_run = False
        
    def printBalanceSheets(self):
        if self.auction_run:
            for agent in self.agent_list:
                agent.printBalanceSheet()
        else:
            print'Auction Not Run Yet.'
            
    def createAuction(self,auction_type,params,bidders):
        auction = None
        if auction_type == 'ascending':
            auction = ascendingAuction(agents = bidders, 
                                        num_bids_thresh = params[0],
                                        reserve = params[1],
                                        bidIncrement = params[2])
        elif auction_type == 'descending':
            auction = descendingAuction(agents = bidders,
                                        num_itr_thresh = params[0],
                                        reserve = params[1],
                                        decrement = params[2])
                                            
        elif auction_type == 'one shot':
             auction = oneShotAuction(agents = bidders,
                                      nprice = params[0],
                                      reserve = params[1]);
        else:
            print '----ERROR----'
            print 'UNKNOWN AUCTION TYPE: {0}'.format(auction_type)
            return None
        
        return auction
            
    def runAuction(self):
        #call the base class function
        super(sequentialAuctions,self).runAuction()
        
        #carry out specialization
        self.auction_run = 1
        
        self.winners = []
        
        num_auctions = len(self.auction_parameters)
        
        print'Number of sequential auctions: {0}'.format(num_auctions)
        
        #iterate over the auctions
        for auction_idx in range(num_auctions):
            auction_type = self.auction_parameters[auction_idx][0]
            print ' '
            print '---------------------------------------------------'
            print '\tRunning Auction: {0}'.format(auction_idx)
            print '\tType: {0}'.format(auction_type)
            
            auction_params = self.auction_parameters[auction_idx]
            bidders = []
            auction = []
            
            #get the specific bidder
            #and give the agents the auction type
            for agent in self.agent_list:
                agent.auctionSchedule.append(auction_type)
                bidders.append(agent.getAgent(auction_type))
            
            #create the particular auction
            auction = self.createAuction(auction_type=auction_type, params=auction_params[1:],bidders=bidders)
            auction.runAuction()
            
            self.winners.append(auction.winners)
            
            #notify agents
            for i in range(len(self.agent_list)):
                if auction.winners[i]==1:
                    self.agent_list[i].won.append(1)
                else:
                    self.agent_list[i].won.append(0)
                self.agent_list[i].v.append(bidders[i].v)
                self.agent_list[i].final_price.append(auction.final_price)
                
                    
    def graphAgentProfitBar(self):
        """
        Bar Chart of Participating Agent Profit
        """
        if self.auction_run:
            fig = figure()
            ax = fig.add_subplot(111)
            
            profits = []
            agent_names = []
            auctionSchedule = []
            num_auctions = len(self.auction_parameters)
            
            
            for agent in self.agent_list:
                agent_names.append(agent.name)
                profits.append(agent.profitTotal())
                
            for auction_idx in range(num_auctions):
                auctionSchedule.append(self.auction_parameters[auction_idx][0])
                
            title_string='Agent Profit:\n {0} Sequential Auctions: {1}'.format(num_auctions,auctionSchedule)
                
            # the x locations for the groups    
            ind = numpy.arange(len(agent_names))    
            
            # width of the bars
            width = .35    
            
            # the bars
            rects1 = ax.bar(ind, profits,width,color='r')
            
            ax.set_ylabel('Profit')
            #ax.set_title('Agent Profit')
            ax.set_title(title_string)
            ax.set_xticks(ind + (width/2))
            ax.set_xticklabels(agent_names)
                
            plt.show()
            
            
        else:
            print'Cannot Graph Agent Profit: Auction not yet Run.'
            
            
    def graphPercentWon(self):
        """
        Pie Graph of Percentage of Auction Each Agent Won.
        """
        if self.auction_run:
            figure(1, figsize=(6,6))
            ax = axes([0.1, 0.1, 0.8, 0.8])
            
            labels = []
            fracs = []
            explode = []
            
            for agent in self.agent_list:
                labels.append(agent.name)
                fracs.append(agent.numAuctionsWon())
                explode.append(0.05)
                
            pie(fracs,explode=explode,labels=labels,autopct='%1.1f%%',shadow=True)
            title('Percentage of Auctions Won by each Agent',
               bbox={'facecolor':'0.8', 'pad':5})
            
            show() 
                
        else:
            print'Cannot Graph Percent Won: Auction not yet Run.'
            
            
            
            
                
                
                
            