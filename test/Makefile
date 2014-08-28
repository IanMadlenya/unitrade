cc = g++ 


ifeq ($(mode), debug)
	CCFLAGS = -std=c++11 -O0  -g  -Wall
else ifeq ($(mode),release)
	CCFLAGS = -std=c++11 -O3  -Wall
else
	mode = profile
	CCFLAGS = -std=c++11 -O3  -pg -Wall
endif


INC = -I. -I../ -I../library/ -I../stock -I../futures -I../option 
INCCTP = -I../externFUT/ctp/include -I../externFUT/ctp 
INCDBF = -I../externSTK/quote/dbf/ -I../externSTK/quote/
INCTS = -I../timeseries/stsdb 
INCCITICS = -I../externSTK/order/citics  -I../externSTK/order

LIBDIR=-L. -L../ -L../library
#LIBDIRCTP = -LexternFUT/ctp/lib/ -LexternFUT/ctp


LINKMAIN = -ltrade 
LINK = -lbin -lm -ldl -lpthread
LINKDBF = -ldbf
LINKCITICS = -lcitics


#LIBSCTP = -Wl,-rpath=externFUT/ctp/lib

.PHONY: clean
all:  cal




#CTPHEAD = externFUT/ctp/MdSpi.h   externFUT/ctp/TraderSpi.h
#CTPSRC = externFUT/ctp/MdSpi.cpp  externFUT/ctp/TraderSpi.cpp

LIBBIN = libbin.a
LIBTRADE = libtrade.a
#LIBCTP = libctp.a
LIBDBF = libdbf.a
LIBCITICS = libcitics.a

cal:
	$(cc) $(CCFLAGS) $(LIBDIR) $(INC) $(INCDBF) testCal.cpp $(LINKMAIN) -o testCal

clean:
	rm -f *.o
	rm -rf *~ *dSYM
	rm -f testCal



	
