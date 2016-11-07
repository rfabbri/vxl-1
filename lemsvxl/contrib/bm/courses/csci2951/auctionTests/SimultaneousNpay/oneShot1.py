from auctionSimulator.simultaneous.simultaneousAgents import oneShotAgent
from auctionSimulator.simultaneous.simultaneousAuctions import oneShotAuction

def main():
    a=oneShotAgent(name="mybot")
    b=oneShotAgent(name="yourbot")
    c=oneShotAgent(name="randomBot")
    #d=agents.simN(name="constBot",vObject=agents.constantValuation(c=1), bObject=agents.smartConstBid(c=.9))
    auction=oneShotAuction()
    auction.attachAgents(list([a,b,c]))
    # auction.attachAgents(list([a,b,c,d]))
    
    auction.runAuction()
    
    auction.printBalanceSheets()
    
    

if __name__ == "__main__":
    main()