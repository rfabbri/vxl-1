"""
this is /auctionSimulator/hw4/utilities.py

Author:    Brandon A. Mayer
Date:      11/19/2011

"""
import numpy
from auctionSimulator.hw4.utilities import *
# plot a histogram of the form we will encounter

#samples = numpy.random.normal(loc=20,scale = 2,size=1000)
#hist, binEdges = numpy.histogram(samples,bins=range(0,51),density=True)
#
#plotDensityHistogram(hist=hist,binEdges=binEdges,xlabel='price',ylabel='probability',title=r'Test Histogram $\mu = 20, \sigma=2$')

m = 5
mu = [30,22,12,8,3]
sigma = [5]*m

priceDistribution = []
for good in xrange(m):
    randomPrices = numpy.random.normal(loc=mu[good],scale=sigma[good],size=10000)
    priceDistribution.append(numpy.histogram(randomPrices,bins=range(0,51),density=True))

plotMarginalDensityHistograms(priceDistribution)