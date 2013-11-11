class simAuction(auction):
    """
    Specialization for
    """
    def __init__(self,args={'m':5,'name':'Anonymous simAuction', 'agents': []}):
        super(simAuction, self).__init__(args)
        
    def type(self):
        return 'Simultaneous Auction'
    
    def runAuction(self, args={}):
        """
        ARGUMENTS:
            verbose    :=    boolean flag indicating to stream online progress to std out
            
            nPrice     :=    Integer specifying N'th price rule. Default is SECOND PRICE
        """
        self.bids = {}
        
        verbose = True
        nPrice = 2
        
        if verbose in args:
            verbose = args['verbose']
            
        if nPrice in args:
            nPrice = args['nPrice']
        
        if verbose:
            print 'Running Auction'
            print 'Auction Name: {0}'.format(self.name)
            print 'Auction Type: {0}'.format(self.type())
            print 'Auction ID: {0}'.format(self.id)
            print 'Number of Participating Agents: {0}'.format(len(self.agents))
            self.printAgentNames()
                
            for agent in self.agentDict.itervalues():
                bidTemp = agent.bid()
                bid[agent.id] = bidTemp
                print"Agent {0}, id {1}, bids: {2}".format(agent.name,agent.id,bidTemp)
                
            arrayIds = numpy.atleast_1d([id for id in bid.iterkeys()])
            arrayBids = numpy.atleast_2d([b for b in bid.itervalues()])
            
                
                
            
        
        
        self.auctionRun = True