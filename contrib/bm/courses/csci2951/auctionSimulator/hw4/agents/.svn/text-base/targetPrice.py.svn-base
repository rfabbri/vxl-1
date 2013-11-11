"""
this is /auctionSimulator/hw4/targetPrice.py

Author: Brandon Mayer
Date: 11/17/2011

Specialized agent class to replicate targetPrice from
Yoon and Wellman (2011)
"""
from agentBase import *


class targetPrice(agent):
    def __init__(self, pricePrediction = [], m = 5, v_min = 1, v_max = 50,name="Anonymous targetPrice"): 
        super(targetPrice,self).__init__(m,v_min,v_max,name)
        
        self.pricePrediction = pricePrediction
        
    def type(self):
        return "targetPrice"
    
    def setPricePrediction(self,pricePrediction = []):
        self.pricePrediction = pricePrediction
    
    def SS(self,args={}):
        """
        Helper function (don't call this one) that processes the given
        price prediction and returns a bid
        
        args = {'pricePrediction': numpy array}
        """
        if 'pricePrediction' in args:
            pricePrediction = args['pricePrediction']
            
            [opt_bundle_idx, opt_bundle, predictedSurplus] = self.acq(pricePrediction)
                
            if (numpy.array(pricePrediction) == float('inf')).any():
                # This shouldn't happen for target price but just incase
                # we should still return the best bid
                priceInfZero = numpy.array(pricePrediction)
                priceInfZero[numpy.nonzero(priceInfZero == float('inf'))[0]] = 0
                return numpy.array([p for p in itertools.imap(operator.mul, opt_bundle,priceInfZero)])
            else:
                return numpy.array([p for p in itertools.imap(operator.mul, opt_bundle, pricePrediction)])
        else:
            return numpy.array([0]*self.l)
    def bid(self,args={}):
        """
        Bid is a vector of predicted prices.
        """
        if 'pointPricePrediction' in args:
            #bid the prices for the optimal bundle
            return self.SS(args)
        elif self.pricePrediction:
            return self.SS({'pricePrediction':self.pricePrediction})
        else:
            warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.targetPrice.bid\n" +\
                      "A point price prediction was not specified as an argument and " +\
                      "this instance has no stored prediction.\n"+\
                      "Agent id {0} will bid zero price for all items".format(self.id)
            sys.stderr.write(warning)
            #bit nothing if you have no price information
            return numpy.array([0]*self.m)