all: control

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


SRC =  control.cpp control.h
LIB = libsecurity.a
OBJ = $(SRC:.cpp=.o)

INC = -I. -I.. -I../security -I../security/crypto  -I../library -I../security/openssl-1.0.1i/include
LINC = -L. -L.. -L../security -L../library -L../security/openssl-1.0.1i/

LINK = -ldl -lsecurity  -lssl -lcrypto

.PHONY: system clean

libsecurity:
	make -C ../security libsecurity

control: libsecurity
	$(cc) $(CCFLAGS) control.cpp $(INC) $(LINC) $(LINK)  -o control

clean:
	rm -rf *dSYM
	rm -rf *.a *.o control
