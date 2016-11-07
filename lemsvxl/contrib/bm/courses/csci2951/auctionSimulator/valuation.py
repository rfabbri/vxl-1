'''
This is /auctionSimulator/valuation.py

Author: Brandon A. Mayer
Date: 10/25/2011

A file containing functors to generate valuations 
for bidding agents for a specific item.

'''
class randomValuation(object):
    def __init__(self,floor=0,ceil=1):
        self.name="randomValuation"
        self.floor=floor
        self.ceil = ceil
        
    def __call__(self):
        import random
        return random.uniform(self.floor,self.ceil)
        
class constantValuation(object):
    def __init__(self, c=.5):
        self.name = "constantValuation"
        self.c = c;
    def __call__(self):
        #print'constantValuation valuation = {0}'.format(self.c)
        return self.c 