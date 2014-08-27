all: aes enc dec full

INC= -I../../library -I../openssl-1.0.1i/include/
LINK= -L../openssl-1.0.1i/ -ldl

enc:
	g++ -std=c++11 -Wall -Wextra -ggdb -o enc crypto-enc.cpp  rsacrypto.cpp  $(INC) $(LINK) -lcrypto

dec:
	g++ -std=c++11 -Wall -Wextra -ggdb -o dec crypto-dec.cpp  rsacrypto.cpp  $(INC) $(LINK) -lcrypto

full:
	g++ -std=c++11 -Wall -Wextra -ggdb -o full crypto-combine.cpp  rsacrypto.cpp  $(INC) $(LINK) -lcrypto

aes:
	g++ -std=c++11 -Wall -Wextra -ggdb -o aes aes.cpp  rsacrypto.cpp $(INC) ../openssl-1.0.1i/libcrypto.a



clean:
	rm -rf *.dSYM
	rm -f *.o aes enc dec full

