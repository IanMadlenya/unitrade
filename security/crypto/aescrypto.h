#ifndef _aescrypt_H_
#define _aescrypt_H_ 1

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "openssl/evp.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "bin.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

namespace binsec {

//const int AES_BLOCK_SIZE = 16;  // manually set block size to 256 bits
const int SALT_SIZE = 8;
/**
 * Create an 256 bit key and IV using the supplied key_data. salt can be added
 *for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/

class AesCrypto {
 public:
  int Init(std::string passphrase, std::string file = "");
  std::string decrypt(std::string enc = "");
  std::string encrypt(std::string);
  int Save(std::string filename = "");
  int Load(std::string filename = "");
  int Shuffle();
  ~AesCrypto() {
    if (pass)
      free(pass);
    if (plain_text)
      free(plain_text);
    if (encrypted_text)
      free(encrypted_text);
    EVP_CIPHER_CTX_free(e_ctx);
    EVP_CIPHER_CTX_free(d_ctx);
    EVP_cleanup();
  }
 private:
  int nrounds;
  EVP_CIPHER_CTX *e_ctx;
  EVP_CIPHER_CTX *d_ctx;
  // size_t salt_len;
  size_t pass_len;
  unsigned char salt[SALT_SIZE];
  unsigned char *pass {nullptr};
  std::string encrypted_file_name;
  std::string msg;
  std::string decrypted_msg;
  std::string encrypted_msg;
  unsigned char *plain_text {nullptr};
  size_t plain_text_len;
  unsigned char *encrypted_text {nullptr};
  size_t encrypted_text_len;
  unsigned char key[EVP_MAX_KEY_LENGTH], iv[EVP_MAX_IV_LENGTH];
};

int AesCrypto::Save(std::string filename) {
  if (filename != "") encrypted_file_name = filename;
  ofstream ofs;
  ofs.open(encrypted_file_name);
  ofs.write((char *)&salt[0], SALT_SIZE);
  ofs.write((char *)encrypted_text, encrypted_text_len);
  ofs.close();
  return 0;
}

int AesCrypto::Load(std::string filename) {
  if (filename != "") encrypted_file_name = filename;
  ifstream ifs;
  ifs.open(encrypted_file_name);
  ifs.read((char *)&salt[0], SALT_SIZE);
  encrypted_text_len = bin::FileLeftBytes(ifs);
  if (encrypted_text)
    free(encrypted_text);
  encrypted_text = (unsigned char *)malloc(encrypted_text_len * sizeof(char));
  auto i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, pass, pass_len,
                          nrounds, key, iv);
  if (i != 32) {
    printf("Key size is %d bits - should be 256 bits\n", i * 8);
    return -1;
  }

  ifs.read((char *)encrypted_text, encrypted_text_len);
  ifs.close();
  return 0;
}

int AesCrypto::Shuffle() {
  memcpy(&salt[0], bin::RandomString(SALT_SIZE).c_str(), SALT_SIZE);
  auto i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, pass, pass_len,
                          nrounds, key, iv);
  if (i != 32) {
    printf("Key size is %d bits - should be 256 bits\n", i * 8);
    return -1;
  }
  return 0;
}

int AesCrypto::Init(std::string passphrase, std::string filename) {
  pass_len = passphrase.size()+1;
  pass = (unsigned char*) malloc( sizeof(char)*pass_len );
  memcpy(pass, passphrase.c_str(), pass_len);
 
  bin::Date mydate;
  nrounds = mydate.month + 5;
  if (filename != "") /*this is usually used to initialize decrypter*/
  {
    encrypted_file_name = filename;
    Load(encrypted_file_name);
  } else {
    Shuffle();
  }

  /*
   * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the
   * supplied key material.
   * nrounds is the number of times the we hash the material. More rounds are
   * more secure but
   * slower.
   */
  e_ctx = EVP_CIPHER_CTX_new();
  d_ctx = EVP_CIPHER_CTX_new();
  EVP_CIPHER_CTX_init(e_ctx);
  EVP_CIPHER_CTX_init(d_ctx);
  return 0;
}

/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
std::string AesCrypto::encrypt(std::string plain) {
  msg = plain;
  plain_text = (unsigned char*) malloc (sizeof(char)*plain.size());
  memcpy(plain_text, plain.c_str(), plain.size()+1);
   
  plain_text_len = plain.size() + 1; /*include terminate null*/

  /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1
   * bytes */
  int c_len = plain_text_len + AES_BLOCK_SIZE, f_len = 0;
  encrypted_text = (unsigned char *)malloc(c_len * sizeof(char));

  /* shuffle salt */
  // memcpy(&salt[0], bin::RandomString(SALT_SIZE).c_str(), SALT_SIZE);
  Shuffle();
 
  EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
   
  /* update ciphertext, c_len is filled with the length of ciphertext generated,
    *len is the size of plaintext in bytes */
  EVP_EncryptUpdate(e_ctx, encrypted_text, &c_len, plain_text, plain_text_len);
  /* update ciphertext with the final remaining bytes */
  EVP_EncryptFinal_ex(e_ctx, encrypted_text + c_len, &f_len);
  encrypted_text_len = c_len + f_len;
  encrypted_msg = std::string(reinterpret_cast<char *>(encrypted_text));
  return encrypted_msg;
}


/*
 * Decrypt *len bytes of ciphertext
 */
std::string AesCrypto::decrypt(std::string enc) {
  if (enc != "") {
    encrypted_msg = enc;
    encrypted_text_len = enc.size();
  }
  /* because we have padding ON, we must allocate an extra cipher block size of
   * memory */
  int p_len = encrypted_text_len, f_len = 0;
  if (plain_text != nullptr) 
  {
    free(plain_text);
    plain_text = nullptr;
  }
  plain_text = (unsigned char *)malloc(p_len + AES_BLOCK_SIZE);
  EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);
  //EVP_DecryptInit_ex(&d_ctx, NULL, NULL, NULL, NULL);
  // cout<<status<<endl;
  EVP_DecryptUpdate(d_ctx, plain_text, &p_len, encrypted_text,
                    encrypted_text_len);
  EVP_DecryptFinal_ex(d_ctx, plain_text + p_len, &f_len);

  plain_text_len = p_len + f_len;
  decrypted_msg = std::string(reinterpret_cast<char *>(plain_text));
  return decrypted_msg;
}
}

#endif

/*
int main(int argc, char **argv) {

  const char *input =
      "\nWho are you ?\nI am the 'Doctor'.\n'Doctor' who ?\nPrecisely!";

  binsec::AesCrypto aes, raes;
  aes.Init(argv[1]);
  raes.Init(argv[1]);

  cout << aes.encrypt(input) << endl;
  aes.Save("test.bin");
  raes.Load("test.bin");
  cout << raes.decrypt() << endl;

  return 0;
}
*/
