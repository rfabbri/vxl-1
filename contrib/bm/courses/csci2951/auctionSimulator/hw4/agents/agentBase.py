'''
This is /auctionSimulator/hw4/agents.py

Author: Brandon A. Mayer
Date: 11/13/2011

A file containing agents and auctions to reproduce Yoon and Wellman
'''
import itertools
import numpy
import operator
import random 
import sys,traceback

def powersetBinary(setLength):
    """
    Function to enumerate the elements of a powerset in binary form.
    e.g. powersetBinary(5) will list assign a unique binary vector forall 
    2**5=32 possible bundles from [0 0 0 0 0] to [1 1 1 1 1]
    """
    return numpy.atleast_2d([bin for bin in itertools.product([False,True],repeat=setLength)]).astype(bool)

class agent(object):
    """
    Base Class for agents
    """
    #unique identifier for the agent
    nextId = 0
    def __init__(self, m = 5, v_min = 1, v_max = 50, name = "Anonymous Agent Base"):          
        """
        Base class for agents from (Yoon & Wellman, 2011).
        INPUTS:
            m            := the target number of time slots for the agent to acquire
            
            v_min        := the minimum valuation for a time slot
            
            v_max        := the maximum valuation for a time slot
        
        MEMBER VARIABLES DEFINED BY __init__:
            self.m                           = m
            
            self.v_min                       = v_min
            
            self.v_max                       = v_max
             
            self.l                           = Target number of time slots agent needs to acquire.
                                                   Scalar drawn uniformly from {1,...m}
                           
            self.v                           = Vector of valuations for individual goods(not bundles)
                                                   Each element is drawn uniformly from {v_min,...,v_max}
                                                   and the resultant is sorted in descending order.
                                                   Note: len(self.v) = self.m
                           
            self.pointPricePrediction        = None
            
            self.distributionPricePrediction = None
                           
        MEMBER METHODS:
            __init__(self, m = 5, v_min = 1, v_max = 50, name = "Anonymous Agent Base")
            type(self)
            bundleFromIndex(self,index)
            reset(self, m = 5, v_min=1, v_max = 50)
            validateBundles(self,bundles)
            validatePriceVector(self,priceVector)
            allBundles(self)
            valuation(self, bundles, validate = True)
            cost(self, bundles, price, validate = True)
            surplus(self, bundles, price, validate=True)
            printAllValuations(self)
            printAllSummary(self,price_vector)
            acq(self,price_vector)
            bid(self,args={}) = 0
            setPricePredictionDistribution
            loadPricePredictionDistribution
            centerBinAvgFromHist()
                           
        NOTES:
            Though I do not store an explicit array of possible bundles
            to save space, we can imagine that there exists a list of bundles, each
            is a binary vector where a 1 in the t^th position indicates that the
            t^th good is included in the bundle. 
            
            If we were to enumerate all bundles, we could count from 0->(self.l-1) 
            e.g. self.l = 3 implies a list of
            [0, 0, 0]
            [0, 0, 1]
            ...
            [1, 1, 1]
            
            I assume big Endianness, that is the msb of the binary representation indicates
            slot one the lsb is then the (self.m - 1)^st good
            
            e.g. [1,0,0] is a bundle containing the first slot and has an index into our "virtual"
            bundle list of 4.
            
            If we want to convert an index into a bundle, just convert the index to binary
            and create the appropriately sized list, padding with zeros as necessary and 
            dictated by self.m
        """
        self.id = agent.nextId
        agent.nextId += 1
        self.auctionRun=False
        
        self.name = name
        self.m = m
        self.v_min = v_min
        self.v_max = v_max
        
        # draw target number of time slots and valuations
        # of completion times
        self.l = numpy.random.random_integers(low=1, high=m)
        self.v = numpy.random.random_integers(low=v_min, high=v_max, size=self.m)
        self.v.sort()
        self.v = list(self.v)[::-1]
        
        self.distributionPricePrediction = None
        self.pointPricePrediction = None
    
        
    def reset(self, m = 5, v_min=1, v_max = 50):
        self.auctionRun = False
        self.m = m
        self.v_min = v_min
        self.v_max = v_max
        
        
        # draw target number of time slots and valuations
        # of completion times
        self.l = numpy.random.random_integers(low=1, high=m)
        self.v = numpy.random.random_integers(low=v_min, high=v_max, size=self.m)
        self.v.sort()
        self.v = list(self.v)[::-1]
        
    def type(self):
        return "Base Agent"
    
    def validateBundles(self,bundles):
        """
        Validation of arbitrary (or list of) bundle(s) 
        """
        bundles = numpy.atleast_2d(bundles)
        
        for bundle in bundles:
            if ((bundle) > 1).any() or (bundle < 0).any():
                err = "----ERROR----\n" +\
                  "auctionSimulator.hw4.agents.validateBundles\n" +\
                  "Bundle: {0}\n".format(bundle) +\
                  "is not a vaild binary bundle."
                traceback.print_stack()
                sys.exit(err)
            elif numpy.atleast_1d(bundle).shape[0] != self.m:
                err = "----ERROR----\n" +\
                  "auctionSimulator.hw4.agents.checkBundles\n" +\
                  "Bundle Length: {0}\n".format(numpy.atleast_1d(bundle).shape[0]) +\
                  "not equal to self.l = {0}.".format(self.l)
                traceback.print_stack()
                sys.exit(err)

        return True
                
    def validatePriceVector(self,priceVector):
        """
        Validation of an arbitrary price vector.
        """
        priceVector = numpy.atleast_2d(priceVector)
        
        if priceVector.shape[0] != 1:
            err = "----ERROR----\n" +\
                  "auctionSimulator.hw4.agents.validatePriceVector\n" +\
                  "Bundle: {0}\n".format(bundle) +\
                  "is not a vaild binary bundle."
            traceback.print_stack()
            sys.exit(err)
            
        elif priceVector.shape[1] != self.m:
            err = "----ERROR----\n" +\
                  "auctionSimulator.hw4.agents.validatePriceVector\n" +\
                  "len(priceVector) = {0} !=".format(priceVector.shape[1]) +\
                  " self.m = {0}".format(self.m)
            traceback.print_stack()
            sys.exit(err)
        else:
            return True
        
    def validateMarginalPriceDistribution(self,priceDistribution, tol = 0.00001):
        """
        Validation of an arbitrary marginal price distribution.
        Marginal distributions over goods are stored as a list of tuples.
        The First item of the tuple should be an array of 
        probability containing the probability density
        The second item in the tuple should be an array
        enumerating the bin edges
        
        There should be one tuple per good, that is
        len(priceDistribution) == self.m
        NOTE:
            If bins are [1,2,3,4] then the first bin
            contains all values within the range [1,2)
            (including 1 excluding 2), [2,3) and 
            [3,4] notice the last bin edge is inclusive
        """
        if len(priceDistribution) != self.m:
            err = "----ERROR----\n" +\
                  "auctionSimulator.hw4.agents.validateMarginalPriceDistribution\n" +\
                  "len(priceDistribution) = {0} !=" +\
                  " self.m = {1}".format(len(priceDistribution),self.m)
            traceback.print_stack()
            sys.exit(err)
            
        for idx in xrange(self.m):
            hist, bin_edges = priceDistribution[idx]
            
            
            if not numpy.abs( numpy.sum(hist*numpy.diff(bin_edges),dtype=numpy.float) 
                          - numpy.float(1.0) ) <= tol:
                err = "----ERROR----\n" +\
                  "auctionSimulator.hw4.agents.validateMarginalPriceDistribution\n" +\
                  "Price Distribution {0} sum = {0} does not sum to 1.0.".format(idx,numpy.sum(hist*numpy.diff(bin_edges)))

                traceback.print_stack()
                sys.exit(err)
                            
        return True
        
       
    def allBundles(self):
        """
        Return a numpy 2d array of all possible bundles that the agent can
        bid on given the number of auctions.
        
        The Rows represent the bundle index.
        
        The Columns Represent the good index.
        """
        return powersetBinary(self.m)
    
    def valuation(self, bundles, validate = True):
        """
        Calculate the valuation of a given set of bundles
        """
        if validate:
            self.validateBundles(bundles)
            
        bundles = numpy.atleast_2d(bundles)
        
        cs = [numpy.atleast_1d(i) for i in itertools.imap(numpy.cumsum,bundles)]
        
        valuation = []
        for bundle in cs:
            if bundle[-1] < self.l:
                valuation.append(0)
            else:
                t = numpy.nonzero(bundle >= self.l)[0][0]
                valuation.append(self.v[t])
                
        return numpy.atleast_1d(valuation)
            
    
    def cost(self, bundles, price, validate = True):
        """
        Calculate the cost for a given set of bundles and prices
        """
        if validate:
            self.validateBundles(bundles)
            self.validatePriceVector(price)
            
        bundles = numpy.atleast_2d(bundles)
        price = numpy.atleast_1d(price)
        
        if price.dtype != numpy.dtype('float'):
            price = price.astype(numpy.float)
            
        if (price == float('inf')).any():
            # if there are items which are unobtainable (cost = inf)
            # then the cost for the bundles containing that good should be inf
            # but the bundles not containing those goods should be the price 
            # of the other goods
            
            # lists are mutable, deep copy the original price vector
            # in order to preserve      
            
            unobtainableGoods = numpy.nonzero(price == float('inf'))[0]
            
            # make a deep copy so not to mutate func. argument
            priceInfZero = numpy.atleast_1d(numpy.array(price))
            priceInfZero[unobtainableGoods] = 0
            
            cost = []
            for idx in xrange(bundles.shape[0]):
                if (bundles[idx][unobtainableGoods] == 0).all():
                    cost.append(numpy.dot(bundles[idx],priceInfZero))
                else:
                    cost.append(float('inf'))
                    
            return numpy.atleast_1d(cost)
        else:
            return numpy.atleast_1d([numpy.dot(bundle,price) for bundle in bundles])
        
    def surplus(self, bundles, price, validate=True):
        """
        Calculate the surplus for a given (2d numpy which can have a single row) 
        array of bundles and prices. Surplus equals valuation less cost.
        """
        if validate:
            self.validateBundles(bundles)
            self.validatePriceVector(price)
            
        #if you've validated price and bundles, here you won't need to in the valuation or cost functions
        #if you didn't want to validate here, you prob won't want to in valuation or cost either, so set to false
        return numpy.atleast_1d([c for c in itertools.imap(operator.sub,self.valuation(bundles,validate=False),self.cost(bundles,price,validate=False))])
        
    def bundleFromIndex(self,index):
        #convert to decimal rather than enumerating power set       
            binList=[]
            n = index
            while n > 0:
                binList.insert(0,n%2)
                n = n >> 1
                      
            # just checking the index wasn't
            # past the maximum bundle
            if len(binList) > self.m: raise ValueError, "Error: Dec-Binary Conversion"
            
            for i in xrange(self.m-len(binList)):
                binList.insert(0,0)
                
            return binList
                           
    def printAllValuations(self):
        print "Target Number of Slots: {0}".format(self.l)
        print "Valuation Vector: {0}".format(self.v)
        print "Bundle   |    Valuation"
        bundles = self.allBundles()
#        valuations = self.allValuation()
        valuations = self.valuation(bundles)
        
        for i in range(len(bundles)):
            print "{0}    {1}".format(bundles,valuations)
                
    def printAllSummary(self,args={}):
        """
        Calculate and print a list of bundle-surplus pairs
        """
        print "Agent Name: {0}".format(self.name)
        print "Agent ID: {0}".format(self.id)
        print "Agent Type: {0}".format(self.type())
        print "Target Number of Time Slots: {0}".format(self.l)
        print "Valuation Vector: {0}".format(self.v)
          
        if 'priceVector' in args:
            priceVector = args['priceVector']    
            print "Price Vector: {0}".format(args['priceVector'] )    
            print "Bundle | Valuation | Cost | Surplus"
            bundles = self.allBundles()
            valuation = self.valuation(bundles)
    #        valuation = self.allValuation()
    #        cost = self.allCost(price_vector=price_vector)
    #        surplus = self.allSurplus(price_vector=price_vector)
            cost = self.cost(bundles, args['priceVector'])
            surplus = self.surplus(bundles,args['priceVector'])
            for i in range(len(bundles)):
                print "{0}    {1:5}    {2:5}    {3:5}".format(bundles[i].astype(numpy.int),valuation[i],cost[i],surplus[i])
            [opt_idx,opt_bundle,optSurplus] = self.acq(args['priceVector'])
            print""
            print"Optimal Bundle Index: {0}".format(opt_idx)
            print"Optimal Bundle: {0}".format(opt_bundle)
            print"Optimal Surplus: {0}".format(optSurplus)
            print"Bid: {0}".format(self.bid(args={'pointPricePrediction':args['priceVector']}))
            
        elif 'priceDistribution' in args:
            expectedPriceVector = self.pointExpectedValFromDist(args['priceDistribution'])
            print "Expected Value Of Distribution: {0}".format(expectedPriceVector)
            print "Bundle | Expected Valuation | Expected Cost | Expected Surplus"
            bundles = self.allBundles()
            valuation = self.valuation(bundles)
            cost = self.cost(bundles, expectedPriceVector)
            surplus = self.surplus(bundles, expectedPriceVector)
            for i in range(len(bundles)):
                print "{0}    {1:5}    {2:5}    {3:5}".format(bundles[i].astype(numpy.int),valuation[i],cost[i],surplus[i])
            [opt_idx,opt_bundle,optSurplus] = self.acq(expectedPriceVector)
            print""
            print"Expected Optimal Bundle Index: {0}".format(opt_idx)
            print"Expected Optimal Bundle: {0}".format(opt_bundle)
            print"Expected Optimal Surplus: {0}".format(surplus[opt_idx])
            print"Bid: {0}".format(self.bid(args={'distributionPricePrediction':args['priceDistribution']}))
           
    def acq(self,price_vector,validate=True):
        """
        Solve for optimal acquisition given price vector.
        INPUTS:
            Predicted Price Vector
        OUTPUTS:
            1. Index into a list of bundles returned by self.allBundles()
            2. The Bundle Computed directly by decimal to binary conversion
               to side step bundle enumeration.
        """
        
        # The cost and surplus functions should handle inf prices
#        surplus = self.allSurplus(price_vector=price_vector)

        surplus = []
        if validate:
            surplus = self.surplus(self.allBundles(), price_vector)
        else:
            surplus = self.surplus(self.allBundles(), price_vector, validate=False)  
            
            
        bundle_idx_list = numpy.nonzero(surplus == numpy.max(surplus))[0]
        
        #initialize to zero
        #if can't find an optimal bundle don't bid...
        #(this shouldn't happen unless the optimal bid is not to bid but 
        # this is here just incase.)
        bundle_idx = 0
        binList = [0]*self.m
        
        if len(bundle_idx_list) == 1:
            bundle_idx = bundle_idx_list[0]       
            binList = self.bundleFromIndex(bundle_idx)
            
        # Pick the bundle that finishes the task first
        # (minimal maximum valuation bundle)
        else:
            t = float('inf')
            
            for idx in bundle_idx_list:
                #get the bundle
                temp_bin_list = self.bundleFromIndex(idx)
                cs = numpy.cumsum(temp_bin_list)
                #t_new = numpy.nonzero(cs >= self.l)[0][0]
                t_new = (numpy.array(cs) >= self.l).nonzero()
                if t_new[0].any():
                    t_new = t_new[0][0]
                    if t_new < t:
                        t = t_new
                        bundle_idx = idx
                        binList = temp_bin_list
                        
                #t_new = numpy.nonzero(numpy.array(bundle)>=self.l)[0][0]
        
        return bundle_idx, binList, surplus[bundle_idx]
    
    def setDistributionPricePrediction(self,distributionPricePrediction = None, validate = True, tol=0.00001):
        """
        Helper function interface with verify option.
        """
        if distributionPricePrediction:
            if validate:
                if self.validateMarginalPriceDistribution(priceDistribution = distributionPricePrediction, tol=tol):
                    self.distributionPricePrediction = distributionPricePrediction
                    return True
                else:
                    warning = "----WARNING----\n" +\
                              "auctionSimulator.hw4.agents.agentBase.agent.setDistributionPricePrediction\n" +\
                              "Could not validate target price prediction distribution."+\
                              "Will not set price distribution for agent id: {0}".format(self.id)
                    sys.stderr.write(warning)
                    return False
            else:
                self.distributionPricePrediction = distributionPricePrediction
                return True
            
        return False
    
    def setPointPricePrediction(self, pointPricePrediction = None, validate=True):
        """
        Helper function interface with verify option.
        """
        if pointPricePrediction:
            if validiate:
                if self.validatePriceVector(pointPricePrediction):
                    self.pointPricePrediction = pointPricePrediction
                    return True
                else:
                    warning = "----WARNING----\n" +\
                              "auctionSimulator.hw4.agents.agentBase.agent.setPointPricePrediction\n" +\
                              "Could not validate target price prediction distribution."+\
                              "Will not set price distribution for agent id: {0}".format(self.id)
                    sys.stderr.write(warning)
                    return False
            else:
                self.pointPricePrediction = pointPricePrediction
                return True 
        else:
            return False
        
        
    def loadPricePredictionDistribution(self,pricePredictionFilename = None,validate=True):
        if pricePredictionFilename:

            npzfile = numpy.load(pricePredictionFilename)
            if 'priceDistribution' in npzfile:
                priceDistribution = npzfile['priceDistribution']
                
                self.distributionPricePrediction = []
                for idx in xrange(len(priceDistribution)):
                    self.distributionPricePrediction.append( (npzfile['priceDistribution'][idx][0],npzfile['priceDistribution'][idx][1]) )
                    
                if validate:
                    return self.validateMarginalPriceDistribution(self.distributionPricePrediction)
                else:
                    return True
            else:
                return False
                
        else:
            return False
       
    @staticmethod
    def centerBinAvgFromHist(hist = None, binEdges = None, tol=0.00001):
        """
        A helper function for computing the average value from a histogram
        """
                
        if not isinstance(hist,numpy.ndarray) or not isinstance(binEdges,numpy.ndarray):
            warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.agentBase.centerBinAvgFromHist()\n" +\
                      "hist and binEdges must be numpy.ndarray s, returning None!"
            sys.stderr.write(warning)
            return None
        
        if not numpy.abs( numpy.sum(hist*numpy.diff(binEdges),dtype=numpy.float)
                          - numpy.float(1.0) ) <= tol:
            warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.agentBase.centerBinAvgFromHist()\n" +\
                      "the provided histogram is not considered a density within specified tolerance, returning None!"
            sys.stderr.write(warning)
            return None
        
        if binEdges.shape[0] != (hist.shape[0] + 1):
            warning = "----WARNING----\n"+\
                      "auctionSimulator.hw4.agents.agentBase.centerBinAvgFromHist()\n" +\
                      "the provided histogram and bins are not of the correct shape!"
            sys.stderr.write(warning)
            return None
        
        return numpy.dot( hist, .5*(binEdges[:-1]+binEdges[1:]) )      
    
    @staticmethod
    def pointExpectedValFromDist(distributions, tol=0.00001):
        
        marginalValues = []
        
        for hist, bins in distributions:
            
            expectedValue = agent.centerBinAvgFromHist(hist=hist,binEdges=bins,tol=tol)
            
            if expectedValue:
                marginalValues.append(agent.centerBinAvgFromHist(hist=hist,binEdges=bins,tol=tol))
            else:
                marginalValues.append(0)
            
        return numpy.atleast_1d(marginalValues)
            
        
        
        
    def bid(self,args={}):
        """
        Abstract method for agent base class
        """
        return []
        
        
        