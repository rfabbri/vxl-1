from auctionSimulator import*

def main():
    randomBot1 = agents.simN(name="randomBot1")
    randomBot2 = agents.simN(name="randomBot2")
    constBot1 = agents.simN(name="constBot1",
                            vObject=agents.constantValuation(c=1),
                            bObject=agents.smartConstBid(c=.6))
                            
    auction = auctions.simN(k=10000,n=2,
        agents=list([randomBot1,randomBot2,constBot1]))
    auction.runAuction()
    randomBot1.printAuctionSummary()
    randomBot2.printAuctionSummary()
    constBot1.printAuctionSummary()
    
    auction.printWinners()
    
    auction.graphPercentWon()
    auction.graphAgentProfit()
    
if __name__ == "__main__":
    main()    