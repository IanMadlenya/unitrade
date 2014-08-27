cc = g++ -Wall
#cc=clang++ -Wall

UNAME = $(shell uname -s) 


ifeq ($(mode),release)
		CCFLAGS = -std=c++11 -O3  -Wall
else ifeq ($(mode),debug)
		CCFLAGS = -std=c++11 -O0 -g -Wall
else
		mode = profile
		CCFLAGS = -std=c++11 -O3  -pg -Wall
endif

ifeq ($(UNAME),Darwin )
	dylinker = -Xlinker -rpath -Xlinker ../hdf5/lib 

else ifeq ($(UNAME),Linux )
 	dylinker = -Wl,-rpath=../hdf5/lib 
endif



INC = -I. -I../ -I../hdf5/include -I../../library
LIBDIR=-L. -L../hdf5/lib -L../../library
LINK = -lhdf5 -lhdf5_hl -ltbb $(dylinker) 
LINKSTSDB = -lbin -lstsdb

.PHONY: clean tags
all:  tag $(LIBSTSDB) import output
HEAD = ../ComHeader.h HDFengine.h ../timeseries.h
SRC =  HDFengine.cpp
MAINSRC = import.cpp
EXAMSRC = output.cpp

LIBSTSDB = libstsdb.a

OBJ = $(SRC:.cpp=.o)

.cpp.o: $(HEAD)
	@echo [CC] $< $@
	$(cc) $(CCFLAGS) $(INC) -c $< -o $@

$(LIBSTSDB): $(OBJ)
	@echo [ar] $(LIBSTSDB) 
	ar rcs $(LIBSTSDB) $(OBJ)

import:  $(LIBSTSDB) $(MAINSRC)
	@echo [cc] import
	$(cc) $(INC) $(LIBDIR) $(CCFLAGS) $(MAINSRC) $(LINKSTSDB) $(LINK) -o import

output:  $(LIBSTSDB) $(EXAMSRC)
	@echo [cc] output
	$(cc) $(INC) $(LIBDIR) $(CCFLAGS) $(EXAMSRC) $(LINKSTSDB) $(LINK) -o output



tag:
	@echo [tag]
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .


clean:
	rm -f *.o
	rm -f *~
	rm -f *.a
	rm -f import output
	rm -f tags
