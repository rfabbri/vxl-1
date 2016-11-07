from auctionSimulator.descending.descendingAgents import *
from auctionSimulator.descending.descendingAuctions import *
from auctionSimulator import valuation

def main():
    """
    Ascending First Price auction with three ranndom bidders.
    """
    d0 = descendingAgent(name="d0",
                         vObject = randomValuation(),
                         bObject = geometricBidDescending() )
    
    d1 = descendingAgent(name="d1",
                         vObject = randomValuation(),
                         bObject = geometricBidDescending() )
    
    d2 = descendingAgent(name="d2",
                         vObject = randomValuation(),
                         bObject = geometricBidDescending() )
    
    auction = descendingAuction(agents=[d0,d1,d2])
    
    auction.runAuction()
    
    auction.printBalanceSheets()

if __name__ == "__main__":
    main()