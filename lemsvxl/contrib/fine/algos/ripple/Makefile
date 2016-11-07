
#Makefile for 

SHELL =    /bin/sh

CC = CC 

INC =  /usr/include/CC

INCSOL = 


# If you want to run a debugger on these programs, remove the -s from LDFLAGS
# and change the -O in CFLAGS to -g.

LDFLAGS = -lm

CCFLAGS = -g +w -DMVEDEBUG
CFLAGS = -g +w -DMVEDEBUG
# -pta instantiates the whole templ. class
GLFLAGS = -lgl 

REPOSITORY_DIR = ./ptrepository

RIPOBJECTS = main.o pnmutil.o LinkedContour.o ProbMap.o RippleFilter.o

.SUFFIXES: .c .cc .cxx .C

# how to compile a C source file:
.c.o:
    $(CC) $(CFLAGS) -c $<


# how to compile a C++ source file:
.cc.o:
    $(CC) $(CCFLAGS) -c $<

# how to compile a C++ source file:
.cxx.o:
    $(CC) $(CCFLAGS) -c $<

# Targets 

all: ripple 

ripple: ${RIPOBJECTS}
    $(CC) $(CFLAGS) -o ripple ${RIPOBJECTS}  $(LDFLAGS)


depend:    
    @echo "Making dependencies\n"
    makedepend -m -I$(INC) -- $(CFLAGS) -- *.cc *.cxx *.c

cleanall:
    /bin/rm -rf $(REPOSITORY_DIR) core *.o 
    
tar:
    /bin/tar -cvf mmvs.tar *.cc *.hh Makefile





# DO NOT DELETE THIS LINE -- make depend depends on it.

LinkedContour.o: LinkedContour.hh /usr/include/CC/iostream.h
LinkedContour.o: /usr/include/memory.h mveDList.hh mveDTLink.hh mveDLink.hh
LinkedContour.o: mveAssert.hh /usr/include/stdlib.h /usr/include/standards.h
LinkedContour.o: /usr/include/sgidefs.h mveDListIter.hh
ProbMap.o: /usr/include/stdio.h /usr/include/standards.h
ProbMap.o: /usr/include/sgidefs.h /usr/include/string.h /usr/include/math.h
ProbMap.o: /usr/include/CC/iostream.h /usr/include/memory.h ProbMap.hh
RippleFilter.o: /usr/include/CC/iostream.h /usr/include/memory.h
RippleFilter.o: mveDListIter.hh mveDTLink.hh mveDLink.hh mveAssert.hh
RippleFilter.o: /usr/include/stdlib.h /usr/include/standards.h
RippleFilter.o: /usr/include/sgidefs.h mveDList.hh RippleFilter.hh ProbMap.hh
RippleFilter.o: LinkedContour.hh
main.o: /usr/include/CC/iostream.h /usr/include/memory.h
main.o: /usr/include/CC/fstream.h pnmutil.h /usr/include/stdio.h
main.o: /usr/include/standards.h /usr/include/sgidefs.h LinkedContour.hh
main.o: mveDList.hh mveDTLink.hh mveDLink.hh mveAssert.hh
main.o: /usr/include/stdlib.h ProbMap.hh RippleFilter.hh
pnmutil.o: /usr/include/stdlib.h /usr/include/standards.h
pnmutil.o: /usr/include/sgidefs.h /usr/include/stdio.h /usr/include/strings.h
pnmutil.o: /usr/include/sys/types.h pnmutil.h
