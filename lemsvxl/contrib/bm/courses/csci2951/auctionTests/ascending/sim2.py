from auctionSimulator.ascending.ascendingAgents import *
from auctionSimulator.ascending.ascendingAuctions import *
from auctionSimulator import valuation

def main():
    """
    Ascending First Price Auction with three incremental bidders.
    """
    a=ascendingAgent(name="Incremental0",
                     vObject=valuation.randomValuation(),
                     bObject=smartBidAskAscending())
    b=ascendingAgent(name="Incremental1",
                     vObject=valuation.randomValuation(),
                     bObject=smartBidAskAscending())
    
    c=ascendingAgent(name="Incremental2",
                     vObject=valuation.randomValuation(),
                     bObject=smartBidAskAscending())
    
    auction = ascendingAuction(agents=[a,b,c],N=1)
    
    auction.runAuction()
    
#    auction.printBidHistory()
    
    auction.printAuctionResults()
    
    a.printBalanceSheet()
    b.printBalanceSheet()
    c.printBalanceSheet()
    

if __name__ == "__main__":
    main()