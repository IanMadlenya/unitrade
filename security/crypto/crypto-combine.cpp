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
    if (argc!=5)
    {
        cerr<<"Usage: full <privatekey> <publickey> <password> <message>"<<endl;
        return -1;
    }
    binsec::RSACrypto crypto(argv[2]);
    string msg(argv[4]);

    // Encrypt the message with RSA
    // Note the +1 tacked on to the string length argument. We want to encrypt the NUL terminator too. If we don't,
    // we would have to put it back after decryption, but it's easier to keep it with the string.

    if( crypto.encrypt(msg) != 0) {
        fprintf(stderr, "Encryption failed\n");
        return 1;
    }

    binsec::RSACrypto decrypto(argv[1], argv[3]);
    std::string ret;
     if( (ret=decrypto.decrypt(crypto.full_encrypted_msg, crypto.full_encrypted_msg_len)) == "") {
        fprintf(stderr, "Decryption failed\n");
        return 1;
    }
    cout<<ret<<endl;
   
    // Print the encrypted message as a base64 string
   return 0;
}
