#include <stdio.h>
#include <iostream>
#include <string>
#include "openssl/ssl.h" 
//#include "base64.h"
#include "rsacrypto.h"
#include "fstream"

//#define PRINT_KEYS

using namespace std;

int main(int argc, char* argv[]) {
    
    if (argc!=3)
    {
        cerr<<"Usage: dec <privatekey> <password>"<<endl;
        return -1;
    }
    binsec::RSACrypto crypto(argv[1], argv[2]);


    printf("Decrypted message: \n");
    cout<< crypto.decrypt("code.bin")<<endl;
  
    return 0;
}
