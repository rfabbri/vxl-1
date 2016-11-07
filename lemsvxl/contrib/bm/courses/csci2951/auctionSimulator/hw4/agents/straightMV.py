"""
this is /auctionSimulator/hw4/straightMV.py

Author: Brandon Mayer
Date: 11/17/2011

Specialized agent class to replicate straightMV from
Yoon and Wellman (2011)
"""
from targetMV import *

class straightMV(targetMV):
    """
    straightMV bids marginal values for all goods.
    
    Inherits from targetMV only need to override type(), SS() and bid functions
    see targetMV.py and agent.py for additional member functions/variables 
    """
    def type(self):
        return "straightMV"
    
    #def SS(self, bundle, pricePrediction, validate=True):
    def SS(self, args={}):
        """
        Calculate the marginal values of all goods for auction
        given the predicted prices.
        
        NOTE:
            Bid on all available goods, don't solve acq
        """
        validate = True
        
        if 'validate' in args:
            validate = args['validate']
            
        pricePrediction = []
        if 'pointPricePrediction' in args:
            pricePrediction = args['pointPricePrediction']
            if validate:
                self.validatePriceVector(pricePrediction)
            
            marginalValueBid = []
            for idx in xrange(self.m):
                tempPriceInf = numpy.array(pricePrediction).astype(numpy.float)
                tempPriceInf[idx] = float('inf')
                tempPriceZero = numpy.array(pricePrediction)
                tempPriceZero[idx] = 0 
                
                [optIdxInf, optBundleInf, predictedSurplusInf] = self.acq(tempPriceInf, validate=False)
                [optIdxZero, optBundleZero, predictedSurplusZero] = self.acq(tempPriceZero, validate=False)
                    
                #this shouldn't happend but just in case.
                if predictedSurplusZero - predictedSurplusInf < 0:
                    marginalValueBid.append(0)
                else:
                    marginalValueBid.append(predictedSurplusZero - predictedSurplusInf)
                
            return numpy.atleast_1d(marginalValueBid).astype('float')
        else:
            warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.{0}.bid\n".format(self.type()) +\
                      "A point price prediction was not specified as an argument and " +\
                      "this instance has no stored prediction.\n"+\
                      "Agent id {0} will bid zero price for all items\n".format(self.id)
            sys.stderr.write(warning)
            return numpy.zeros(self.m)
            