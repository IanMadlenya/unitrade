
#include <iostream>
#include "openssl/aes.h"


#include "aescrypto.h"
using namespace std;

int main(int argc, char **argv) {

  const char *input =
      "\nWho are you ?\nI am the 'Doctor'.\n'Doctor' who ?\nPrecisely!";

  binsec::AesCrypto raes;
 
  raes.Init("abc", "test.bin");
  cout<<raes.encrypt(input)<<endl;
  raes.Save();
  raes.Load();
  cout << raes.decrypt() << endl;
  return 0;
}
