from auctionSimulator.simultaneous.simultaneousAgents import simNAgent
from auctionSimulator.simultaneous.simultaneousAuctions import simNAuction

def main():
    a=simNAgent(name="mybot")
    b=simNAgent(name="yourbot")
    c=simNAgent(name="randomBot")
    #d=agents.simN(name="constBot",vObject=agents.constantValuation(c=1), bObject=agents.smartConstBid(c=.9))
    auction=simNAuction(k=100)
    auction.r=.2
    auction.n=3
    auction.attachAgents(list([a,b,c]))
    # auction.attachAgents(list([a,b,c,d]))
    auction.n=1
    auction.runAuction()
    auction.printAuctionResults()
   
    a.printAuctionSummary()
    b.printAuctionSummary()
    c.printAuctionSummary()
    
    
    auction.graphPercentWon()
    auction.graphAgentProfit()
    
    
    

if __name__ == "__main__":
    main()