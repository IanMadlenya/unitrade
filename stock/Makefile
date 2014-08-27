cc = g++ 


ifeq ($(mode), debug)
	CCFLAGS = -std=c++11 -O0  -g  -Wall
else ifeq ($(mode),release)
	CCFLAGS = -std=c++11 -O3  -Wall
else
	mode = profile
	CCFLAGS = -std=c++11 -O3  -pg -Wall
endif


INC = -I. -I../ -I../library/

LIBDIR=-L. -L../ -L../library
#LIBDIRCTP = -LexternFUT/ctp/lib/ -LexternFUT/ctp


LINKMAIN = -ltrade 
LINK = -lbin -lm -ldl -lpthread




#LIBSCTP = -Wl,-rpath=externFUT/ctp/lib

.PHONY: clean
all:  librisk
HEAD = risk.h
SRC =  risk.cpp 

LIBBIN = libbin.a


OBJ = $(SRC:.cpp=.o)
#OBJCTP = $(CTPSRC:.cpp=.o)

.cpp.o: $(HEAD)
	@echo [CC] $< $@
	$(cc)  $(INC) $(INCTS) $(INCCTP) $(INCDBF) $(INCCITICS) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -rf *~ *dSYM
	rm -f *.a



