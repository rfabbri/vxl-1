import sys,string

fp=open(sys.argv[1])
topM=fp.readlines()
fp.close()

fp=open(sys.argv[2])
vis=fp.readlines()
fp.close()

topMatch={}
visitInfo={}

for d in topM:
    dd=string.split(string.strip(d))
    if len(dd)>=2:
        topMatch[dd[0]]=dd[1]

for d in vis:
    dd=string.split(string.strip(d))
    if len(dd)==10:
        t=[dd[2],string.atof(dd[4]),string.atoi(dd[6]),string.atoi(dd[9])]
        #print t
        visitInfo[dd[0]]=t

sum=0.0
visFracList=[]
minVisFrac=100
maxVisFrac=0
numErrors=0
for d in visitInfo.keys():
    if (topMatch.has_key(d)):
        t1=visitInfo[d][0]
        t2=topMatch[d]
        if (t1 != t2):
            #print "Error: top matches different",d,t1,t2
            numErrors=numErrors+1
        visFrac=100*visitInfo[d][2]/(visitInfo[d][2]+visitInfo[d][3])
        #print visFrac,
        visFracList.append(visFrac)
        sum=sum+visFrac
        if visFrac > maxVisFrac:
            maxVisFrac = visFrac
        if visFrac < minVisFrac:
            minVisFrac = visFrac
    else:
        print "Invalid key?",d
#print "Num Errors",numErrors
#for i in visFracList:
#    print i,
#print " "
#print "Mean",sum/len(visitInfo.keys()),"Min",minVisFrac,"Max",maxVisFrac
perc=sum/len(visitInfo.keys())
print '%5.4f'%perc,'%5.4f'%(maxVisFrac),'%5.4f'%(minVisFrac),numErrors
#print '%5.2f'%perc,'\5.2f%',(maxVisFrac),'\5.2f%',(minVisFrac),'\5.2f%',numErrors
