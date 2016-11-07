"""
this is /auctionSimulator/hw4/utilities.py

Author:    Brandon A. Mayer
Date:      11/19/2011
"""

import numpy
import matplotlib.pyplot as plt
import sys


            
def sampleMarginalDistributions(priceDistributions,args={}):

    method = 'itransform'
    if 'method' in args:
        method = args['method']

    
    if method == 'itransform':
        nSamples = 100
        
        if 'nSamples' in args:
            nSamples = args['nSamples']
        
        #each row is a random sample from the marginal
        #distribution for the good in the corresponding column
        samples = numpy.zeros((nSamples,len(priceDistributions)))
        
        for col in xrange(len(priceDistributions)):
            
            hist,binEdges = priceDistributions[col]  
            cdf = numpy.cumsum(hist)
            
            #draw a new set of number for each marginal
            #distribution
            randomNumbers = numpy.random.uniform(0,1,nSamples)
            for row in xrange(nSamples):
                b = numpy.nonzero(cdf >= randomNumbers[row])[0][0]
                price = binEdges[b]
                samples[row][col]=price
                
                
        return numpy.atleast_2d(samples)
        
        
    else:
        warning = "----WARNING----\n"+\
                  "auctionSimulator.hw4.utilities.sampleMarginalDistributions\n" +\
                  "UKNOWN SAMPLING METHOD RETURNING NONE\n"
        sys.stderr.write(warning)
        return None

def plotDensityHistogram(hist = None, binEdges = None, title= None, xlabel = None, ylabel = None):
        
        if not isinstance(hist,numpy.ndarray) or not isinstance(binEdges,numpy.ndarray):
            warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.agentBase.centerBinAvgFromHist()\n" +\
                      "hist and binEdges must be numpy.ndarray s, returning None\n!"
            sys.stderr.write(warning)
            return None
        
        if binEdges.shape[0] != (hist.shape[0] + 1):
            warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.agentBase.centerBinAvgFromHist()\n" +\
                      "the provided histogram and bins are not of the correct shape\n!"
            sys.stderr.write(warning)
            return None
        
                
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.bar(left=binEdges[:-1],height=hist,width=numpy.diff(binEdges))
        
        if xlabel:
            ax.set_xlabel(xlabel)
        else:
            ax.set_xlabel('bins')
            
        if ylabel:
            ax.set_ylabel(ylabel)
        
        if title:
            ax.set_title(title)
        
        plt.show()
        
def plotMarginalDensityHistograms(distributions, style='subplots', title= None, xlabel = None, ylabel = None):
    
    if style != 'overlay' and style != 'subplots':
        warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.agentBase.plotMarginalDensityHistograms()\n" +\
                      "choices for style parameter are \'overlay\' or \'subplots\'."
        sys.stderr.write(warning)
            
    
    
    if style == 'subplots':
        fig = plt.figure()
        fig.subplots_adjust(hspace=0.5)
        nDist = len(distributions)
        
        for pltIdx in xrange(nDist):
            hist = distributions[pltIdx][0]
            binEdges =distributions[pltIdx][1]
        
            ax = fig.add_subplot((1+nDist)/2,nDist/2,pltIdx+1)
            
            ax.bar(left=binEdges[:-1],height=hist,width=numpy.diff(binEdges))
            
            if xlabel:
                ax.set_xlabel(xlabel)
            else:
                ax.set_xlabel('Price')
            
                
            if ylabel:
                ax.set_ylabel(ylabel)
            else:
                ax.set_ylabel('Pr[price]')
            
            if title:
                ax.set_title(title)
            else:
                ax.set_title('Distribution {0}'.format(pltIdx))
#    elif style == 'overlay':
#        fig = plt.figure()
#        nDist = len(distributions)
#        
#        binEdges = distributions[0][1]
#        for pltIdx in xrange(nDist):
#            hist = distributions[pltIdx][0]
            
            
                
    else:
        warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.agentBase.plotMarginalDensityHistograms()\n" +\
                      "choices for style parameter are \'overlay\' or \'subplots\'."
        sys.stderr.write(warning)
        return None
        
    plt.show()          
        