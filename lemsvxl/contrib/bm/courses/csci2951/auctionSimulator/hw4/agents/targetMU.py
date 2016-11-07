"""
this is /auctionSimulator/hw4/targetMU.py

Author: Brandon Mayer
Date:   11/21/2011

Specialized agent class to replicate targetMU from Yoon and Wellman 2011.
This is just a wrapper around targetMV and accepts a price prediction distribution
and calculates the mean(s) for price prediction
"""
from targetMV import *

class targetMU(targetMV):
    def __init__(self, m = 5,
                 v_min = 1, 
                 v_max = 50,
                 name="Anonymous",
                 pricePredictionDistributionFilename = []):
        
        if pricePredictionDistributionFilename:
            self.loadPricePredictionDistribution(pricePredictionDistributionFilename)
             
        super(targetMU,self).__init__(m,v_min,v_max,name)
        
    def type(self):
        return "targetMU"
            
            
    def SS(self,args={'method':'average'}):
        """
        Calculate the expected marginal price vector given marginal distributions
        over good prices. 
        
        We consider in the average, the price associated with a bin to be the
        bins center. The average is then calculated as summing the product 
        of the bin centers multiplied by the bin probability.
        """
        
        if 'method' in args:
            method = args['method']
        else:
            method = 'average'
            
        if not 'distributionPricePrediction' in args:
            return numpy.zeros(self.m)
        
        marginalAverages = []
        if method == 'average':
            for hist, binEdges in args['distributionPricePrediction']:
                #calculate the average for each marginal good
                marginalAverages.append(self.centerBinAvgFromHist(hist=hist,binEdges=binEdges))
                        
            marginalAverages = numpy.atleast_1d(marginalAverages)
            
            if marginalAverages.shape[0] != self.m:
                warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.targetMU.SS()\n" +\
                      "Calculated point vector has the wrong size.\n" +\
                      "Returning a bid of all zeros."
                sys.stderr.write(warning)
                return numpy.zeros(self.m)
        else:
            warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.targetMU.SS()\n" +\
                      "unknown method specified for extracing point prediction from distribution.\n" +\
                      "Returning a bid of all zeros."
            sys.stderr.write(warning)
            return numpy.zeros(self.m)
        
        # now that we have a point price prediction from the distribution,
        # bid as if we were targetMV. We can do this by just calling and
        # returning the parent bidding strategy profile
        return super(targetMU,self).SS({'pointPricePrediction':marginalAverages})
        
        
    def bid(self, args={}):
        """
        Bid returns the same bid(args) as targetMV except we calculate
        a point price prediction using the predictive distribution.
        """
        
        pricePredictionDistribution = []
        if 'distributionPricePrediction' in args:
            return self.SS({'distributionPricePrediction':args['distributionPricePrediction']})
        elif self.distributionPricePrediction:
            return self.SS({'distributionPricePrediction':self.distributionPricePrediction})
        else:
            warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.{0}.bid\n".format(self.type()) +\
                      "A distribution price prediction was not specified as an argument and " +\
                      "this instance has no stored prediction.\n"+\
                      "Agent id {0} will bid zero price for all items\n".format(self.id)
            sys.stderr.write(warning)
            return numpy.zeros(self.m)
        
        
                