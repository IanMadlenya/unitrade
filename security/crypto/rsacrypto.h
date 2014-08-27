#ifndef _rsacrypt_H_
#define _rsacrypt_H_ 1


#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#include <stdio.h>
#include <string>
#include <string.h>




namespace binsec {

#define RSA_KEYLEN 2048

#define PSUEDO_CLIENT

//#define USE_PBKDF

#define SUCCESS 0
#define FAILURE -1

#define KEY_SERVER_PRI 0
#define KEY_SERVER_PUB 1
#define KEY_CLIENT_PUB 2

class RSACrypto {

 public:
  RSACrypto(std::string pubname, std::string priname, std::string pass);
  RSACrypto(std::string pubname);
  RSACrypto(std::string priname, std::string pass);

  RSACrypto(unsigned char *remotePubKey, size_t remotePubKeyLen);

  ~RSACrypto();

  int rsaEncrypt(const unsigned char *msg, size_t msgLen,
                 unsigned char **encMsg, unsigned char **ek, size_t *ekl,
                 unsigned char **iv, size_t *ivl);

  int rsaDecrypt(unsigned char *encMsg, size_t encMsgLen, unsigned char *ek,
                 size_t ekl, unsigned char *iv, size_t ivl,
                 unsigned char **decMsg);

  int writeKeyToFile(FILE *fd, int key);

  int getRemotePubKey(unsigned char **pubKey);

  int setRemotePubKey(unsigned char *pubKey, size_t pubKeyLen);

  int getLocalPubKey(unsigned char **pubKey);

  int getLocalPriKey(unsigned char **priKey);

  int readLocalPubKey(std::string name);

  int readLocalPriKey(std::string name, std::string pass);

  std::string decrypt(std::ifstream &ifs);

  std::string decrypt(std::string name);

  std::string decrypt(unsigned char *encrypted_msg, size_t len);

  std::string getDecrypt() { return decrypted_msg; }
  int encrypt(std::string msg);
  int save(std::string filename);

  unsigned char *full_encrypted_msg{nullptr};
  int full_encrypted_msg_len{};

 private:
  unsigned char *ek {nullptr};
  unsigned char *iv {nullptr};
  unsigned char *encMsg {nullptr};
  unsigned char *decMsg {nullptr};
  size_t eklen, ivlen;
  unsigned long enclen, declen;
  std::string pass{};
  EVP_PKEY *remotePubKey{};
  EVP_PKEY *localPriKey{};
  EVP_CIPHER_CTX *rsaEncryptCtx{};
  EVP_CIPHER_CTX *rsaDecryptCtx{};
  int init();
  std::string pub_file_name{};
  std::string pri_file_name{};
  std::string code_file_name{};
  std::string msg{};
  std::string encrypted_msg{};
  std::string decrypted_msg{};
};
}
#endif
