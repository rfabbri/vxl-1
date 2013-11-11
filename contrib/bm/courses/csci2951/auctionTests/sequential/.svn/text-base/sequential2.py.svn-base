from auctionSimulator.ascending.ascendingAgents import *
from auctionSimulator.descending.descendingAgents import *
from auctionSimulator.simultaneous.simultaneousAgents import *

from auctionSimulator.sequential.sequentialAgents import *
from auctionSimulator.sequential.sequentialAuctions import *

from auctionSimulator import valuation

def main():
    """
    Ascending First Price auction with three ranndom bidders.
    """
    a0 = ascendingAgent(name="Random0",
                        vObject=valuation.randomValuation(),
                        bObject=randomValidBidAscending())
    
    a1 = ascendingAgent(name="Random1",
                        vObject=valuation.randomValuation(),
                        bObject=smartBidAskAscending())
    
    a2 = ascendingAgent(name="Random2",
                        vObject=valuation.randomValuation(),
                        bObject=randomBidAscending())
    
    a3 = ascendingAgent(name="Random2",
                        vObject=valuation.randomValuation(),
                        bObject=randomBidAscending())
    
    d0 = descendingAgent(name="d0",
                        vObject = randomValuation(),
                        bObject = geometricBidDescending() )
    
    d1 = descendingAgent(name="d1",
                        vObject = randomValuation(),
                        bObject = geometricBidDescending() )
    
    d2 = descendingAgent(name="d2",
                        vObject = randomValuation(),
                        bObject = valuationBidDescending() )
    
    d3 = descendingAgent(name="d2",
                        vObject = randomValuation(),
                        bObject = vLessConstDescending(c=.2) )
       
    oneShot0 = oneShotAgent(name="OneShot0")
    oneShot1 = oneShotAgent(name="OneShot1")
    oneShot2 = oneShotAgent(name="OneShot2")
    oneShot3 = oneShotAgent(name="OneShot2")
    
    agent_dictonary0 = {'ascending':a0,
                         'descending':d0,
                         'one shot':oneShot0 }
    agent_dictonary1 = {'ascending':a1,
                         'descending':d1,
                         'one shot':oneShot1 }
    
    agent_dictonary2 = {'ascending':a2,
                         'descending':d2,
                         'one shot':oneShot2 }
    
    agent_dictonary3 = {'ascending':a3,
                         'descending':d3,
                         'one shot':oneShot3 }
    
    
    s0 = sequentialAgent(name='Sequential 0',agent_dictionary=agent_dictonary0)
    s1 = sequentialAgent(name='Sequential 1',agent_dictionary=agent_dictonary1)
    s2 = sequentialAgent(name='Sequential 2',agent_dictionary=agent_dictonary2)
    s3 = sequentialAgent(name='Sequential 3',agent_dictionary=agent_dictonary3)
    
    num_bids_thresh = 1000
    ascending_reserve = 0.0001
    bidIncrement = 0.01
    
    num_itr_thresh = 1000
    descending_reserve = 1.0
    decrement = 0.01
    
    n = 1
    r = 0.001
    parameters = [['ascending',num_bids_thresh,ascending_reserve,bidIncrement],
                  ['descending',num_itr_thresh,descending_reserve,decrement],
                  ['one shot',n,r],
                  ['ascending',num_bids_thresh,ascending_reserve,bidIncrement],
                  ['ascending',num_bids_thresh,ascending_reserve,bidIncrement],
                  ['one shot',n,r],
                  ['one shot',n,r],
                  ['one shot',n,r]]
    
    auction = sequentialAuctions(agents=[s0,s1,s2,s3],auction_parameters=parameters)
    
    auction.runAuction()
    
#    auction.printBidHistory()
    
    auction.printBalanceSheets()
    
    auction.graphAgentProfitBar()
    
    auction.graphPercentWon()

if __name__ == "__main__":
    main()