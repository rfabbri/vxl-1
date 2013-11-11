from auctionSimulator.ascending.ascendingAgents import *
from auctionSimulator.ascending.ascendingAuctions import *
from auctionSimulator import valuation

def main():
    """
    Ascending First Price auction with three ranndom bidders.
    """
    a=ascendingAgent(name="Random0",
                     vObject=valuation.randomValuation(),
                     bObject=randomBidAscending())
    b=ascendingAgent(name="Random1",
                     vObject=valuation.randomValuation(),
                     bObject=randomBidAscending())
    
    c=ascendingAgent(name="Random2",
                     vObject=valuation.randomValuation(),
                     bObject=randomBidAscending())
    
    d=ascendingAgent(name="Incremental3",
                     vObject=valuation.randomValuation(),
                     bObject=smartBidAskAscending())
    
    e=ascendingAgent(name="RandomValid4",
                     vObject=valuation.randomValuation(),
                     bObject=randomValidBidAscending())
    
    auction = ascendingAuction(agents=[a,b,c,d,e])
    
    auction.runAuction()
    
#    auction.printBidHistory()
    
    auction.printAuctionResults()
    
    auction.printBalanceSheets()
    

if __name__ == "__main__":
    main()