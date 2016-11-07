"""
this is /auctionSimulator/hw4/agents/riskAware.py

Author:    Brandon A. Mayer
Date:      11/23/2011
"""

from agentBase import *
from straightMV import *
from targetMV import *
from targetMVS import *
from targetPrice import *

class riskAware(agentBase):
    """
    Risk aware agent. This agent can adopt several strategies for bidding but
    chooses optimal bundle based on a modified mean variance utility function which
    is a more general case of the acquisition problem proposed by Boyan & Greenwald,2001
    and used in Yoon & Wellman 2011. The mean utility variance function is calculated 
    using upper partial variance instead of variance to account for the asymettric
    effect of price volitility on bundle acquisition
    """
    def __init__(self, m = 5,
                 v_min = 1, 
                 v_max = 50,
                 name="Anonymous",
                 A = 0,
                 bidStrategy='targetPrice'):
        self.A = A
        self.bidStrategy = bidStrategy 
        super(riskAware,self).__init__(m,v_min,v_max,name)
        
        
    def type(self):
        return "riskAware_{0}".format(self.bidStrategy)
    
    @staticmethod
    def upperParialVariance(expectedValueVector = None, 
                            marginalPriceDistributions = None):
        """
        Helper function to compute upper partial variance
        
        UPV = (E_F[price]-price)^2 if price > E_F[price]
                              0                 else
        """
        
        if isinstance(marginalPriceDistributions,list):
            
            upv = []
            for idx in xrange(len(marginalPriceDistributions)):
                
                binEdges = marginalPriceDistributions[idx][1]
                binEdges = numpy.array(binEdges)
                upperPrices = binEdges[numpy.nonzero(binEdges >= expectedValueVector[idx])]
                
                upperPriceDiff = []
                for price in upperPrices:
                    upperPriceDiff.append(expectedValueVector[idx] - price)
                
                #dot product will yeild sum of squares
                upv.append(numpy.dot(upperPriceDiff,upperPriceDiff))
                    
            return numpy.array(upv)
        else:#is a single distribution
            hist, binEdges = marginalPriceDistributions
            binEdges = numpy.array(binEdges)
            
    @staticmethod
    def meanUpvUtil(bundles = None, marginalPriceDistributions = None, A = None):
        """
        Yet another helper. Computes mean upper partial variance utility.
        
        INPUTS:
            bundle                      :=  bit vector indicating which goods are in the bundle
                                            Can be a list of bundles or a single bundle
            
            marginalPriceDistributions  :=  marginal probability distributions over
                                            the prices of goods in the bundle
                                           
            A                           := the risk adversion parameter                            
        """
        if bundle == None or \
           marginalPriceDistributions == None or \
           A == None:
           warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.{0}.meanUpvUtil\n".format(self.type()) +\
                      "One or more arguments are not specified, returning float('-inf') utility."
           sys.stderr.write(warning)  
           return float('-inf')
       
        if isinstance(bundles,list):
            expectedPrices = self.pointExpectedValFromDist(marginalPriceDistributions)
            
            expectedPrices = self.surplus(bundles, expectedPrices)
            
            util = []
            for bundleIdx in xrange(bundles.shape[0]):
                
                #the sum of the upper partial var for this bundle
                sumUpv = 0
                for goodIdx in xrange(expectedPrices.shape):
                    if bundles[bundleIdx][goodIdx]:
                        binEdges = numpy.atleast_1d(marginalPriceDistributions[goodIdx][1])
                        upperPrices = binEdges[numpy.nonzero(binEdges>=expectedPrices[goodIdx])]
                        upperPriceDiff = numpy.array([d for d in itertools(operator.sub,
                                                                           itertools.repeat(expectedPrices[goodIdx],times=upperPrices.shape[0]),
                                                                           upperPrices)])
                        sumUpv += numpy.dot(upperPriceDiff)
                        
                util.append(expectedSurplus[bundleIdx] - .5 *A*sumUpv)
                        
        else:
            #this gives expected prices of all goods
            expectedPrices = self.pointExpectedValFromDist(marginalPriceDistributions)
           
            #returns expected surplus given the bundle
            expectedSurplus = self.surplus(bundles, expectedPrices)
            
            sumUpv = 0
            for idx in xrange(expectedPrices.shape[0]):
                
                #if the good is contained in the bundle, 
                #add its volitility contribution to the upv
                if bundles[idx]:
                    binEdges = numpy.atleast_1d(marginalPriceDistributions[idx][1])
                    upperPrices = binEdges[numpy.nonzero(binEdges>=expectedPrices[idx])]
                    upperPriceDiff = numpy.array([d for d in itertools(operator.sub,
                                                                       itertools.repeat(expectedPrices[idx],times=upperPrices.shape[0]),
                                                                       upperPrices)])
                    
                    #the sum of sqr differences for all prices above the expected price
                    sumUpv += numpy.dot(upperPriceDiff,upperPriceDiff)
                    
            return expectedSurplus - .5*A*sumUpv
                                                                   
                
    def acqMeanUtilUPS(self, marginalPriceDistributions = None, A = None):
        """
        Compute optimal bundle using Mean Utility uper-partial variance
        function.
        We will maximize the utility:
        U = E_F[price] - .5*A*UPV
        
        UPV = (ExpectedSurplus-price)^2 if price > E_F[price]
                              0                 else
        
        A is a free parameter representing the agent's risk "adversness"
        A = 0 -> risk neutral (reduces to acq from Boyan & Greenwald)
        A < 0 -> gabmberl/risk lover, the agent gains utility from taking on
                 extra risk
        A > 0 -> risk adverse bidder, agent values safer bundles
        
        if A = None, we will use the value stored in self.A,
        
        if A = None and self.A = None we use A = 0
        """
        if priceDistribution:
            
            if not A:
                if not self.A:
                    A = 0
                else:
                    A = self.A
            
            allBundles = self.allBundles()
            
            #return the mean - upper paritial variance utilities for each bundles
            utility = self.meanUpvUtil(allBundles, marginalPriceDistributions = marginalPriceDistributions, A = A)
            
            #pick the bundle that maximizes utility
            optBundleIdxList = numpy.nonzero(utility==numpy.max(utility))[0]
            
            optBundle = []
            optExpectedSurplus = []
            if len(optBundleIdxList) == 1:
                optBundle = allBundles[optBundleIdxList[0]]
                optExpectedSurplus = expectedSurplus[optBundleIdxList[0]]
            else: 
                #if we get here it means there are multiple bundles with the same maximal utility
                #pick the one that finishes first ("minimally maximal")
                t = float('inf')
                for idx in optBundleIdxList:
                    tempBinList = allBundles[idx]
                    cs = numpy.cumsum(tempBinList)
                    tNew = (numpy.array(cs) >= self.l).nonzero()
                    if tNew[0].any():
                        tNew = tNew[0][0]
                        if tNew < t:
                            optBundle = allBundles[idx]
                            optExpectedSurplus = expectedSurplus[idx]
                            
            return optBundle, optExpectedSurplus
                            
            
        else:
            warning = "----WARNING----" +\
                      "auctionSimulator.hw4.agents.riskAware.acqMeanUtilUPS\n" +\
                      "No price distribution specified, returning zero bundle.\n"
            sys.stderr.write(warning)          
            return numpy.zeros(self.m)
        
        
    def SS(self,args={}):
        if not 'distributionPricePrediction' in args:
            return numpy.zeros(self.m)
        
        optBundle = None
        optExpectedSurplus = None
        if 'A' in args:
            optBundle, optExpectedSurplus = acqMeanUtilUPS(marginalPriceDistributions=args['distributionPricePrediction'],
                                                           A = args['A'])
        else:
            optBundle, optExpectedSurplus = acqMeanUtilUPS(marginalPriceDistributions=args['distributionPricePrediction'])
            
        if self.bidStrategy == 'targetPrice':
            #bid the expected values for the optimal bundle according to mean - upper partial variance function
            expectedPrices = self.pointExpectedValFromDist(distributions=args['distributionPricePrediction'])
            bid = []
            for goodIdx in xrange(optBundle.shape[0]):
                if optBundle[goodIdx]:
                    bid.append(expectedPrices[goodIdx])
                else:
                    bid.append(0)
            return bid
        elif self.bidStrategy == 'targetMU':
            pass
        else:
            warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.{0}.SS\n".format(self.type()) +\
                      "Agent ID: {0}".format(self.id) +\
                      "self.bidStrategy = {0} is of unknown type, returning a bid of all zeros\n".format(self.bidStrategy)
            sys.stderr.write(warning)
            return numpy.zeros(self.m)
            
    
    def bid(self, args={}):
        """
        Interface to the strategy profile
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
        
    def printAllSummary(self,args={}):
        """
        Override the printAllSummary for this agent
        """
        print "Agent Name: {0}".format(self.name)
        print "Agent ID: {0}".format(self.id)
        print "Agent Type: {0}".format(self.type())
        print "Target Number of Time Slots: {0}".format(self.l)
        print "Valuation Vector: {0}".format(self.v)
        
        if 'priceDistribution' not in args:
            warning = "----WARNING----\n" +\
                      "auctionSimulator.hw4.agents.{0}.printAllSummary\n".format(self.type()) +\
                      "A distribution price prediction was not specified as an argument " +\
                      "cannot print Summary."
            sys.stderr.write(warning) 
            return None
        