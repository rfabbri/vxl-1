from auctionSimulator import*

def main():
    a = agents.simN(name="constBot1",vObject=agents.constantValuation(c=1), bObject=agents.smartConstBid(c=.1))
    b = agents.simN(name="constBot2",vObject=agents.constantValuation(c=1), bObject=agents.smartConstBid(c=.2))
    auction=auctions.simN(k=10,r=.5,n=1,agents=list([a,b]))
    auction.runAuction()
    auction.printAuctionResults()
    a.printProfit()
    
if __name__ == "__main__":
    main()