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
    
    auction = ascendingAuction(agents=[a,b,c],N=2)
    
    auction.runAuction()
    
#    auction.printBidHistory()
    
    auction.printAuctionResults()
    
    a.printBalanceSheet()
    b.printBalanceSheet()
    c.printBalanceSheet()
    

if __name__ == "__main__":
    main()


 