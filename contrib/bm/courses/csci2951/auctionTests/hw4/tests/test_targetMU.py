from auctionSimulator.hw4.agents.targetMU import *
from tempfile import TemporaryFile, NamedTemporaryFile

import numpy
import os
from auctionSimulator.hw4.utilities import plotDensityHistogram
from auctionSimulator.hw4.agents.agentBase import agent

def main():
    m = 5
    priceDistribution = []
    
    myTargetMU = targetMU()
    mu = [5,3,2,1,1]
    sigma = [.2]*m
#    for good in xrange(m):
#        randomPrices = numpy.random.random_integers(0,50,1000)
#        priceDistribution.append(numpy.histogram(randomPrices,bins=range(0,51),density=True))
    for good in xrange(m):
        randomPrices = numpy.random.normal(loc=mu[good],scale=sigma[good],size=10000)
        priceDistribution.append(numpy.histogram(randomPrices,bins=range(0,51),density=True))
        
    print 'average priceDistribution[0] = {0}'.format(agent.centerBinAvgFromHist(priceDistribution[0][0], priceDistribution[0][1]))
#    plotDensityHistogram(priceDistribution[0][0],priceDistribution[0][1],xlabel='price',ylabel='probability',title='Price Histogram')
    
        
        
#    print "priceDistribution = {0}".format(priceDistribution)
    # make a temporary file to test loading distribution from file
    outfile = NamedTemporaryFile()
    outfile2 = NamedTemporaryFile(suffix='.npz')
    
    histArray = []
    binEdgeArray = []
    for tuple in priceDistribution:
        histArray.append(tuple[0])
        binEdgeArray.append(tuple[1])
    
    histArray = numpy.atleast_2d(histArray)
    binEdgeArray = numpy.atleast_2d(binEdgeArray)
    
 
    filename = 'F:\\courses\\fall2011\\csci2951\\hw4\\distTest.npz'
#    outfile = open(filename)
    if not os.path.exists(filename):
        outfile = open(filename,'r+')
        outfile.close()

    numpy.savez(filename, priceDistribution=priceDistribution)
            
    if myTargetMU.loadPricePredictionDistribution(filename, validate=True):
        print "myTargetMU Price Prediction: {0}".format(myTargetMU.distributionPricePrediction)
        print "And was validated."
    
    print 'Bidding With distribution loaded from binary...'
    
    print ""
    myTargetMU.printAllSummary(myTargetMU.pointExpectedValFromDist(myTargetMU.distributionPricePrediction))
    
    print 'myTargetMU.bid() = {0}'.format(myTargetMU.bid())
    
        
    
    
if __name__ == "__main__":
    main()