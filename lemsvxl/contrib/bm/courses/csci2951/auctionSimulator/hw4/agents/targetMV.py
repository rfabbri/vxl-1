"""
this is /auctionSimulator/hw4/targetMV.py

Author: Brandon Mayer
Date: 11/17/2011

Specialized agent class to replicate targetMV from
Yoon and Wellman (2011)
"""
from agentBase import *

class targetMV(agent):
    def __init__(self, m = 5,
                 v_min = 1, v_max = 50,name="Anonymous"): 
        super(targetMV,self).__init__(m,v_min,v_max,name)
        
    def type(self):
        return "targetMV"
    
    def setPricePrediction(self,pricePrediction = []):
        self.pricePrediction = pricePrediction
    
#    def SS(selfs,pricePrediction):
#        if pricePrediction
    
    def SS(self,args={}):
        """
        Calculate a vector of marginal values given a price
        vector.
        """
        validate = True
        
        if 'validate' in args:
            validate = args['validate']
            
        if 'pointPricePrediction' in args:
            pricePrediction = args['pointPricePrediction']
            if validate:
                self.validatePriceVector(pricePrediction)
                
            [optBundleIdx, optBundle, optSurplus] = self.acq(pricePrediction,validate=False)
            
            marginalValueBid = []
            
            for idx in xrange(self.m):
                if optBundle[idx] == 1:
                    tempPriceInf = numpy.array(pricePrediction).astype(numpy.float)
                    tempPriceInf[idx] = float('inf')
                    tempPriceZero = numpy.array(pricePrediction).astype(numpy.float)
                    tempPriceZero[idx] = 0
                    
                    [optIdxInf, optBundleInf, predictedSurplusInf] = self.acq(tempPriceInf,validate=False)
                    [optIdxZero, optBundleZero, predictedSurplusZero] = self.acq(tempPriceZero,validate=False)
                    
                    # this shouldn't happen but just in case       
                    if predictedSurplusZero - predictedSurplusInf < 0:
                        marginalValueBid.append(0)
                    else:
                        marginalValueBid.append(predictedSurplusZero-predictedSurplusInf)
                        
                else:
                    marginalValueBid.append(0)
            
            return numpy.atleast_1d(marginalValueBid).astype('float')
        else:
            return numpy.zeros(self.m)
             
    def bid(self,args={}):
        """
        Bid is a vector of prives given a vector
        of predicted point prices.
        
        Marginal Value:
            Solve acq for each good when good_i is unobtainable (price = inf)
            and when good_i is free (price = 0) and take the difference
        """
        
        if 'pointPricePrediction' in args:
            return self.SS({'pointPricePrediction':args['pointPricePrediction']})
        elif self.pricePrediction:
            return self.SS({'pointPricePrediction': self.pointPricePrediction})
        else:
            warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.{0}.bid\n".format(self.type()) +\
                      "A point price prediction was not specified as an argument and " +\
                      "this instance has no stored prediction.\n"+\
                      "Agent id {0} will bid zero price for all items\n".format(self.id)
            sys.stderr.write(warning)
            return numpy.zeros(self.l)
          