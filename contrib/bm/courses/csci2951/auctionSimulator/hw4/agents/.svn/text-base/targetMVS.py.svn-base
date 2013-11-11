"""
this is /auctionSimulator/hw4/targetMVS.py

Author: Brandon Mayer
Date: 11/18/2011

Specialized agent class to replicate targetMV* (hence MVS stands for MV Star) from
Yoon and Wellman (2011)
"""
from targetMV import *

class targetMVS(targetMV):
    """
    targetMVS is identical to targetMV so we can inherit from targetMV
    and only override the helper bid strategy profile SS.
    
    targetMVS assumes that "all goods outside the taget bundle
    are unavailable" (Yoon & Wellman 2011)
    
    NOTE:
        Because this class inherits from targetMV which inherits from agent,
        all of the usual member functions and variables are defined though not 
        explicitly listed here, check those files.
    """
    def type(self):
        return "targetMVS"
    
    def SS(self, args = {}):
        """
        Calculate a vector of marginal values given a single bundle
        and price vector assuming the price off all goods not in the bundle
        are infinite (unobtainable).
        """
        
        validate = True
        if 'validate' in args:
            validate = args['validate']
            
        if 'pointPricePrediction' in args:
            
            pricePrediction = args['pointPricePrediction']     
            
            if validate:
                self.validatePriceVector(pricePrediction)
                   
            [optBundleIdx, optBundle, optSurplus] = self.acq(pricePrediction,validate=False)
            
            # set the price of all goods not in the optimal bundle to infinity
            # deep copy price to preserve original price vector
            pricePredictionInf = numpy.array(pricePrediction).astype(numpy.float)
            pricePredictionInf[ (numpy.atleast_1d(optBundle) == 0) ] = float('inf')
            
            marginalValueBid = []
            for idx in xrange(self.m):
                if optBundle[idx] == 1:
                    tempPriceInf = pricePredictionInf.astype(numpy.float)
                    tempPriceInf[idx] = float('inf')
                    tempPriceZero = pricePredictionInf
                    tempPriceZero[idx] = 0
                    
                    [optIdxInf, optBundleInf, predictedSurplusInf] = self.acq(tempPriceInf, validate=False)
                    [optIdxZero, optBundleZero, predictedSurplusZero] = self.acq(tempPriceZero, validate=False)
                    
                    #this shouldn't happend but just in case.
                    if predictedSurplusZero - predictedSurplusInf < 0:
                        marginalValueBid.append(0)
                    else:
                        marginalValueBid.append(predictedSurplusZero - predictedSurplusInf)
                else:
                    marginalValueBid.append(0)
                    
            return numpy.atleast_1d(marginalValueBid).astype('float')
        else:
            warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.{0}.bid\n".format(self.type()) +\
                      "A point price prediction was not specified as an argument and " +\
                      "this instance has no stored prediction.\n"+\
                      "Agent id {0} will bid zero price for all items\n".format(self.id)
            sys.stderr.write(warning)
            return numpy.zeros(self.m)
            
        
            