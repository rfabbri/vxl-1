"""
this is /auctionSimulator/hw4/averageMU.py

Author: Brandon Mayer
Date:   11/21/2011

Specialized agent class to replicate targetMU from Yoon and Wellman 2011.
This is just a wrapper around targetMV and accepts a price prediction distribution
and calculates the mean(s) for price prediction
"""

# take targetMU as base so we don't have to rewrite bid function
from targetMU import *
from auctionSimulator.hw4.utilities import sampleMarginalDistributions

class averageMU(targetMU):
    def __init__(self, m = 5,
                 v_min = 1, 
                 v_max = 50,
                 name="Anonymous",
                 pricePredictionDistributionFilename = []):
        
        if pricePredictionDistributionFilename:
            self.loadPricePredictionDistribution(pricePredictionDistributionFilename)
             
        super(averageMU,self).__init__(m,v_min,v_max,name)
        
    def type(self):
        return "averageMU"
                 
    def SS(self,args={}):
        
        if not 'distributionPricePrediction' in args:
            warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.{0}.SS()\n".format(self.type()) +\
                      "unknown method specified for extracing point prediction from distribution.\n" +\
                      "Returning a bid of all zeros.\n"
            sys.stderr.write(warning)
            return numpy.zeros(self.m)
                               
        nSamples = 100
        if 'nSamples' in args:
            nSamples = args['nSamples']
            
        validate = True
        if 'validate' in args:
            validate = args['validate']
            
        if validate:
            self.validateMarginalPriceDistribution(priceDistribution=args['distributionPricePrediction'])
            
        method = 'itransform'
        if 'method' in args:
            method = args['method']
            
        samplingArgs={'method':method,'nSamples':nSamples}
        
        priceSamples = sampleMarginalDistributions(args['distributionPricePrediction'],samplingArgs)
        
        #for each price vector, we calculate the marginal
        #value of the ith good
        avgMV = numpy.zeros(self.m,dtype=numpy.float)
        
        for idx in xrange(priceSamples.shape[1]):
            mv = []
            for s in priceSamples:
                tempPriceInf = numpy.array(s).astype(numpy.float)
                tempPriceInf[idx] = float('inf')
                tempPriceZero = numpy.array(s)
                tempPriceZero[idx] = 0
                
                [optIdxInf, optBundleInf, predictedSurplusInf] = self.acq(tempPriceInf, validate=False)
                [optIdxZero, optBundleZero, predictedSurplusZero] = self.acq(tempPriceZero, validate=False)
                
                if predictedSurplusZero - predictedSurplusInf < 0:
                    mv.append(0)
                else:
                    mv.append(predictedSurplusZero - predictedSurplusInf)
                    
            avgMV[idx]=numpy.mean(mv)
            
        return avgMV
            
                
                
                
                
                
        
        
        
    
        
        
        
    