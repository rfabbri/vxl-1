


CC = gcc 

SRC      = test.c utils.c matrix_lib.c\
        stop.c  snakes_top.c\

                   
PROGRAM = snakes

OBJS      = $(SRC:.c=.o)

LIBDIRS  = -L/vision/projects/kimia/tek-projects/c/cedt/shocks/subpixel-shocks 
LIBUTILS = -L/vision/projects/kimia/tek-projects/c/c-utils
INCDIRS  = -I/vision/projects/kimia/tek-projects/c/cedt/shocks/subpixel-shocks
INCUTILS = -I/vision/projects/kimia/tek-projects/c/c-utils

LIBPATH = $(LIBUTILS) $(LIBDIRS) 
INCPATH = $(INCUTILS) $(INCDIRS) 

DEFINES        = 
CC_OPT        = -O3 
DEBUG            = -g #-pg
CFLAGS        = $(CC_OPT) $(DEBUG) $(DEFINES) $(INCPATH)
LDFLAGS        = $(DEBUG)

LIBS       = -lUtils -lTracedShocks -lc -lm

all:: snakes libSnakes.a


$(PROGRAM): $(OBJS)
    $(CC) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LIBPATH) $(INCPATH) $(LIBS)

libSnakes.a : $(OBJS)
    $(AR) rv $@ $?
    ranlib $@

clean:
    rm -f *.[oOS] *.bak

tidy:
    rm -f *.[oOS] *.bak *~ core run

