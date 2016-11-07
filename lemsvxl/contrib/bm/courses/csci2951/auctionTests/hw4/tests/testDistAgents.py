from auctionSimulator.hw4.agents.straightMU import *
from auctionSimulator.hw4.agents.targetMU import *
from auctionSimulator.hw4.agents.targetMUS import *
from auctionSimulator.hw4.agents.averageMU import *
from auctionSimulator.hw4.agents.riskAware import *


import itertools
import numpy

def main():
    
    #simulate a price Distribution
    m = 5
    muPrice = [5,3,2,1,1]
    sigma = [.2]*m
    priceDistribution = []
    for good in xrange(m):
        randomPrices = numpy.random.normal(loc=muPrice[good],scale=sigma[good],size=10000)
        priceDistribution.append(numpy.histogram(randomPrices,bins=range(0,51),density=True))
        
    myTargetMU = targetMU()
    myTargetMU.printAllSummary({'priceDistribution':priceDistribution})
    print "\n\n"
    
    myTargetMUS = targetMUS()
    myTargetMUS.printAllSummary({'priceDistribution':priceDistribution})
    print "\n\n"
    
    myStraightMU = straightMU()
    myStraightMU.printAllSummary({'priceDistribution':priceDistribution})
    print "\n\n"
    
    myAverageMU = averageMU()
    myAverageMU.printAllSummary({'priceDistribution':priceDistribution})
    print "\n\n"
    
    myRiskAware = riskAware(A = 2)
    myRishAware.printAllSummary({'priceDistribution':priceDistribution})
    print "\n\n"
    
        
if __name__ == "__main__":
    main()