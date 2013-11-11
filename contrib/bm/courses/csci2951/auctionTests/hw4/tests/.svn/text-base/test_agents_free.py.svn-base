from auctionSimulator.hw4.agents.baselineBidder import *
from auctionSimulator.hw4.agents.targetPrice import *
from auctionSimulator.hw4.agents.targetMV import *
from auctionSimulator.hw4.agents.targetMVS import *
from auctionSimulator.hw4.agents.straightMV import *

import itertools
import numpy

def main():
#    p = [10,3,4,2,1]
    p = [0,0,0,0,0]
    
    myTargetPrice = targetPrice()
    
    myTargetPrice.printAllSummary(p)   
        
    print "\n \n \n"
    
    myBaseline = baselineBidder(name="myBaselineAgent")
    
    myBaseline.printAllSummary({'priceVector':p})
    
    print "\n\n\n"
    
    myTargetMV = targetMV(name="myTargetMV")
    
    myTargetMV.printAllSummary({'priceVector':p})
    
    print "\n\n\n"
    
    myTargetMVS = targetMVS(name="myTargetMVS")
    
    myTargetMVS.printAllSummary({'priceVector':p})
    
    print "\n\n\n"
    
    myStraightMV = straightMV(name="myStraightMV")
    
    myStraightMV.printAllSummary({'priceVector':p}) 

if __name__ == "__main__":
    main()