all: libsecurity codegen server

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


SRC =  security.cpp  crypto/rsacrypto.cpp
LIB = libsecurity.a
OBJ = $(SRC:.cpp=.o)

INC = -I. -I.. -Icrypto  -I../library -Iopenssl-1.0.1i/include
LINC = -L. -L.. -L../library -Lopenssl-1.0.1i/

LINK = -ldl -lsecurity  -lssl -lcrypto

.PHONY: system clean


.cpp.o: $(INC) 
	$(cc) $(CCFLAGS) $(INC) -c $< -o $@

libsecurity: $(OBJ)
	ar rcs libsecurity.a $(OBJ)

codegen: libsecurity
	$(cc) $(CCFLAGS) codegen.cpp $(INC) $(LINC) $(LINK)  -o codegen


server: libsecurity
	$(cc) $(CCFLAGS) server.cpp $(INC) $(LINC) $(LINK)  -o server




openssl:
	make -C openssl-1.0.1i clean
	cd openssl-1.0.1i;./config
	make -C openssl-1.0.1i  mode=$(mode)

opensslclean:
	make -C openssl-1.0.1i clean

clean:
	make -C crypto clean
	rm -rf *dSYM
	rm -rf *.a *.o auth check codegen authserver verify server client
