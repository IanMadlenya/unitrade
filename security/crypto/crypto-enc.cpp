#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "openssl/ssl.h" 
//#include "base64.h"
#include "rsacrypto.h"

//#define PRINT_KEYS

using namespace std;

int main(int argc, char* argv[]) 
{
    if (argc!=3)
    {
        cerr<<"Usage: enc <publickey> <message>"<<endl;
        return -1;
    }
    binsec::RSACrypto crypto(argv[1]);
    string msg(argv[2]);

    // Encrypt the message with RSA
    // Note the +1 tacked on to the string length argument. We want to encrypt the NUL terminator too. If we don't,
    // we would have to put it back after decryption, but it's easier to keep it with the string.

    if( crypto.encrypt(msg) != 0) {
        fprintf(stderr, "Encryption failed\n");
        return 1;
    }
    crypto.save("code.bin");
    // Print the encrypted message as a base64 string
   return 0;
}
