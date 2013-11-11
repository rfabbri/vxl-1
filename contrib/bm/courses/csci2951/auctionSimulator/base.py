'''
This is /auctionSimulator/base.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file containing the base classes for agents and
auctions.

'''

class agentBase(object):
    """A base class for agents."""
    def __init__(self,name="anonymous"):
        self.name = name
        
        # a vector of valuations
        self.v=[]
        
        # a vector of bids don't need to keep these but
        # storage is cheap
        self.b=[]
        
        # a bit vector of indicating items won 
        self.won=[] 
        
        # price at which item was acquired
        self.final_price=[]

        #unique id optionally assigned by auction
        self.id = []
        
    def initAgent(self):
        """
        A function for auctions to call in order to clear past results for new auction
        """
        self.v=[]
        self.b=[]
        self.won=[]
        self.final_price=[]
        self.id=[]
        
    def printName(self):
        print self.name
        
    def bid(self,k):
        pass
        
    def printValuation(self):
        pass
    
    
class auctionBase(object):
    def __init__(self,name="Auction Base",agents=[]):
        """A base class for auctions"""
        #name of the auction
        self.name=name
        
        # a list of lists containing the agent index on the row
        # and the columns are the corresponding bid vector
        self.bids=[]
        
        # a list of participating agents
        self.agent_list=[]
        
        # a matrix of bit values one indicates a win
        # for a given auction format left to specific auctions
        self.winners=[]
        
        # final price(s) for the given auction
        self.final_price = []
       
        # flag indicating if the auction had been run
        self.auction_run = False
        
        # the list of agents
        if not isinstance(agents,list) and \
                isinstance(agents, agentBase):
            self.agent_list.append(a)
        elif isinstance(agents,list):
            for agent in agents:
                if isinstance(agent, agentBase):
                    self.agent_list.append(agent)   
        else:
            print 'Couldn\'t initialize agent list'
            self.agent_list=[]
            
    def attachAgents(self, a):        
        """Attatch a list of agents or a single agent"""
        if not isinstance(a,list) \
            and isinstance(a, agentBase):
            self.agent_list.append(a);
        elif isinstance(a,list):
            for agent in a:
                if isinstance(agent, agentBase):
                    self.agent_list.append(agent);
                
                
    def printAgentNames(self):
        print 'Agent Names:'
        if self.agent_list == None:
            print '\tAgent List is Empty'
        else:
            for agent in self.agent_list:
                print '\t%s' % agent.name
                
    def printAgentBids(self):
        if self.auctionRun:
            for agent in self.agent_list:
                agent.printBids()
        else:
            print'Cannot Print Bids: Auction Not yet Run'
            

    def printAgentValuations(self):
        if self.auctionRun:
            for agent in self.agent_list:
                agent.printValuation()  
        else:
            print'Cannot Print Valuations: Auction Not yet Run'
            
    def printAuctionResults(self):
        pass    
    
    def runAuction(self):
        for agent in self.agent_list:
            agent.initAgent()