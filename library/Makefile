all: system libbin

cc = g++
UNAME = $(shell uname -s) 


ifeq ($(mode),release)
	CCFLAGS = -std=c++11 -O3  -Wall
else ifeq ($(mode),profile)
	CCFLAGS = -std=c++11 -O3 -pg -Wall
else
	mode = debug
	CCFLAGS = -std=c++11 -O0  -g -Wall
endif


ifeq ($(UNAME),Darwin )
	sd = gsed
	numcore = $(shell sysctl hw.ncpu | awk '{print $$2}') 
else ifeq ($(UNAME),Linux )
	sd = sed
	numcore = $(shell grep -c ^processor /proc/cpuinfo)
else
	numcore = 4
endif


SRC =  replace.cpp tinyxml2.cpp 
LIB = libbin.a
OBJ = $(SRC:.cpp=.o)


.PHONY: system clean


system:
	$(sd) -i "/NUM_CPU_CORE/c\const int NUM_CPU_CORE=$(numcore);" system.inc

.cpp.o: $(INC) 
	$(cc) $(CCFLAGS) -c $< -o $@

libbin: system $(OBJ)
	ar rcs libbin.a $(OBJ)
	rm *.o

clean:
	rm -rf libbin.a
