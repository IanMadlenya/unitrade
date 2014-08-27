cc = g++ 


ifeq ($(mode), debug)
	CCFLAGS = -std=c++11 -O0  -g  -Wall
else ifeq ($(mode),release)
	CCFLAGS = -std=c++11 -O3  -Wall
else
	mode = profile
	CCFLAGS = -std=c++11 -O3  -pg -Wall
endif


INC = -I. -Ilibrary/ -Istock -Ifutures -Ioption 
INCCTP = -IexternFUT/ctp/include -IexternFUT/ctp 
INCDBF = -IexternSTK/quote/dbf/ -IexternSTK/quote/
INCTS = -Itimeseries/stsdb 
INCCITICS = -IexternSTK/order/citics  -IexternSTK/order

LIBDIR=-L. -Llibrary
#LIBDIRCTP = -LexternFUT/ctp/lib/ -LexternFUT/ctp


LINKMAIN = -ltrade 
LINK = -lbin -lm -ldl -lpthread
LINKDBF = -ldbf
LINKCITICS = -lcitics


#LIBSCTP = -Wl,-rpath=externFUT/ctp/lib

.PHONY: clean
all:  unitrade
HEAD = global.h ComHeader.h StkHeader.h StkCompute.h StkExecute.h quote.h order.h dbf.h
SRC =  risk/risk.cpp  common.cpp stock/stk.cpp futures/fut.cpp global.cpp util.cpp StkThread.cpp FutThread.cpp stock/StkCompute.cpp stock/StkExecute.cpp futures/FutCompute.cpp futures/FutExecute.cpp
MAINSRC = main.cpp

#CTPHEAD = externFUT/ctp/MdSpi.h   externFUT/ctp/TraderSpi.h
#CTPSRC = externFUT/ctp/MdSpi.cpp  externFUT/ctp/TraderSpi.cpp

DBFSRC = externSTK/quote/quote.cpp externSTK/quote/dbf/readdbf.cpp
CITICSSRC = externSTK/order/order.cpp 

LIBBIN = libbin.a
LIBTRADE = libtrade.a
#LIBCTP = libctp.a
LIBDBF = libdbf.a
LIBCITICS = libcitics.a

OBJ = $(SRC:.cpp=.o)
#OBJCTP = $(CTPSRC:.cpp=.o)
OBJDBF = $(DBFSRC:.cpp=.o)
OBJCITICS = $(CITICSSRC:.cpp=.o)

.cpp.o: $(HEAD)
	@echo [CC] $< $@
	$(cc)  $(INC) $(INCTS) $(INCCTP) $(INCDBF) $(INCCITICS) $(CCFLAGS) -c $< -o $@


#$(LIBCTP): $(OBJCTP) $(CTPHEAD) $(CTPSRC)
#	ar -r -c -s $(LIBCTP) $(OBJCTP)


$(LIBBIN):
	make -C library mode=$(mode)

$(LIBTRADE): $(OBJ) 
	@echo [ar] $(LIBTRADE)
	ar rcs $(LIBTRADE) $(OBJ)

$(LIBDBF): $(OBJDBF)
	@echo [ar] $(LIBDBF)
	ar rcs $(LIBDBF) $(OBJDBF)

$(LIBCITICS): $(OBJCITICS)
	@echo [ar] $(LIBCITICS)
	ar rcs $(LIBCITICS) $(OBJCITICS)


unitrade: $(LIBBIN) $(LIBDBF) $(LIBCITICS) $(LIBTRADE)
	@echo [cc] unitrade
	$(cc) $(INC) $(INCTS) $(INCDBF) $(INCCITICS) $(CCFLAGS) $(MAINSRC) $(LIBDIR) $(LINKMAIN) $(LINKCITICS) $(LINKDBF)  $(LINK)   -o unitrade

clean:
	rm -f *.o
	rm -rf *~ *dSYM
	rm -f unitrade
	rm -f *.a
	rm -f externFUT/ctp/*.o
	rm -f externSTK/quote/*.o externSTK/quote/dbf/*.o externSTK/order/*.o
	make -C library clean
	make -C security clean
	make -C control clean
	make -C timeseries clean
	make -C risk clean
	make -C stock clean
	make -C futures clean
