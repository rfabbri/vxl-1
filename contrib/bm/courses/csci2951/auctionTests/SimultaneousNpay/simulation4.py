from auctionSimulator import*

def main():
    a=agents.simN(name="negBot",vObject=agents.constantValuation(c=.5),bObject=agents.constantBid(c=.7))
    b=agents.simN(name="smartBot",vObject=agents.constantValuation(c=1),bObject=agents.randomBid(floor=.5))
    auction = auctions.simN(k=10,agents=list([a,b]))
    auction.runAuction()
    
    a.printAuctionSummary()
    b.printAuctionSummary()
    
    auction.graphPercentWon()
    #auction.graphAgentProfit()
    
    
    
    

if __name__ == "__main__":
    main()