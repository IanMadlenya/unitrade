#include "rsacrypto.h"
#include "fstream"
#include <cstdio>
#include <iostream>

using namespace std;

namespace binsec {

// EVP_PKEY* Crypto::localKeypair;
std::streamsize available(std::istream &is) {
  std::streampos pos = is.tellg();
  is.seekg(0, std::ios::end);
  std::streamsize len = is.tellg() - pos;
  is.seekg(pos);
  return len;
}

RSACrypto::RSACrypto(std::string pubname, std::string priname, std::string ps) {
  // localKeypair  = NULL;
  remotePubKey = NULL;
  pub_file_name = pubname;
  pri_file_name = priname;
  pass = ps;
  if (init()) exit(-1);
}

RSACrypto::RSACrypto(std::string pubname) {
  // localKeypair  = NULL;
  remotePubKey = NULL;
  pub_file_name = pubname;
  pri_file_name = "";
  if (init()) exit(-1);
}

RSACrypto::RSACrypto(std::string priname, std::string ps) {
  // localKeypair  = NULL;
  remotePubKey = NULL;
  pub_file_name = "";
  pri_file_name = priname;
  pass = ps;
  if (init()) exit(-1);
}

RSACrypto::RSACrypto(unsigned char *remotePubKey, size_t remotePubKeyLen) {
  this->remotePubKey = NULL;
  setRemotePubKey(remotePubKey, remotePubKeyLen);
  if (init()) exit(-1);
}

RSACrypto::~RSACrypto() {
  if (ek) free(ek);
  if (iv) free(iv);
  if (encMsg) free(encMsg);
  if (decMsg) free(decMsg);
  if (full_encrypted_msg) free(full_encrypted_msg);
  EVP_PKEY_free(localPriKey);
  EVP_PKEY_free(remotePubKey);
  EVP_CIPHER_CTX_cleanup(rsaEncryptCtx);
  EVP_CIPHER_CTX_cleanup(rsaDecryptCtx);
  free(rsaEncryptCtx);
  free(rsaDecryptCtx);
}

int RSACrypto::rsaEncrypt(const unsigned char *msg, size_t msgLen,
                          unsigned char **encMsg, unsigned char **ek,
                          size_t *ekl, unsigned char **iv, size_t *ivl) {
  size_t encMsgLen = 0;
  size_t blockLen = 0;

  *ek = (unsigned char *)malloc(EVP_PKEY_size(remotePubKey));
  *iv = (unsigned char *)malloc(EVP_MAX_IV_LENGTH);
  if (*ek == NULL || *iv == NULL) return FAILURE;
  *ivl = EVP_MAX_IV_LENGTH;

  *encMsg = (unsigned char *)malloc(msgLen + EVP_MAX_IV_LENGTH);
  if (encMsg == NULL) return FAILURE;

  if (!EVP_SealInit(rsaEncryptCtx, EVP_aes_256_cbc(), ek, (int *)ekl, *iv,
                    &remotePubKey, 1)) {
    return FAILURE;
  }

  if (!EVP_SealUpdate(rsaEncryptCtx, *encMsg + encMsgLen, (int *)&blockLen,
                      (const unsigned char *)msg, (int)msgLen)) {
    return FAILURE;
  }
  encMsgLen += blockLen;

  if (!EVP_SealFinal(rsaEncryptCtx, *encMsg + encMsgLen, (int *)&blockLen)) {
    return FAILURE;
  }
  encMsgLen += blockLen;

  EVP_CIPHER_CTX_cleanup(rsaEncryptCtx);

  return (int)encMsgLen;
}

int RSACrypto::rsaDecrypt(unsigned char *encMsg, size_t encMsgLen,
                          unsigned char *ek, size_t ekl, unsigned char *iv,
                          size_t ivl, unsigned char **decMsg) {
  size_t decLen = 0;
  size_t blockLen = 0;
  EVP_PKEY *key;
  *decMsg = (unsigned char *)malloc(encMsgLen + ivl);
  if (decMsg == NULL) return FAILURE;
  key = localPriKey;
  // ek = nullptr;

  // ek = (unsigned char*)malloc( EVP_PKEY_size(localPriKey) );
  if (ek == NULL) {
    std::cerr << "ERROR: ek allocate error" << std::endl;
    return FAILURE;
  }

  ekl = EVP_PKEY_size(localPriKey);

  if (!EVP_OpenInit(rsaDecryptCtx, EVP_aes_256_cbc(), ek, ekl, iv, key)) {
    ERR_print_errors_fp(stderr);
    std::cerr << "ERROR: EVP_Openinit" << std::endl;
    return FAILURE;
  }

  if (!EVP_OpenUpdate(rsaDecryptCtx, (unsigned char *)*decMsg + decLen,
                      (int *)&blockLen, encMsg, (int)encMsgLen)) {
    ERR_print_errors_fp(stderr);
    return FAILURE;
  }
  decLen += blockLen;

  if (!EVP_OpenFinal(rsaDecryptCtx, (unsigned char *)*decMsg + decLen,
                     (int *)&blockLen)) {
    ERR_print_errors_fp(stderr);
    return FAILURE;
  }
  decLen += blockLen;

  EVP_CIPHER_CTX_cleanup(rsaDecryptCtx);

  return (int)decLen;
}

std::string RSACrypto::decrypt(std::string name) {
  ifstream ifs;
  ifs.open(name);
  auto ret = decrypt(ifs);
  ifs.close();
  return ret;
}

std::string RSACrypto::decrypt(std::ifstream &ifs) {
  ek = (unsigned char *)malloc(eklen * sizeof(unsigned char));
  iv = (unsigned char *)malloc(ivlen * sizeof(unsigned char));
  ifs.read((char *)ek, eklen);
  ifs.read((char *)iv, ivlen);
  enclen = available(ifs);
  encMsg = (unsigned char *)malloc(enclen * sizeof(unsigned char *));
  ifs.read((char *)encMsg, enclen);
  declen = rsaDecrypt(encMsg, enclen, &ek[0], eklen, &iv[0], ivlen,
                      (unsigned char **)&decMsg);
  if (declen == -1) {
    fprintf(stderr, "Decryption failed\n");
    return "";
  }
  decrypted_msg = string(reinterpret_cast<char *>(decMsg));
  return decrypted_msg;
}

std::string RSACrypto::decrypt(unsigned char *encrypted_msg, size_t len) {
  full_encrypted_msg = (unsigned char *)malloc(sizeof(char) * len);
  memcpy(full_encrypted_msg, encrypted_msg, len);
  ek = (unsigned char *)malloc(eklen * sizeof(unsigned char));
  iv = (unsigned char *)malloc(ivlen * sizeof(unsigned char));
  memcpy(ek, full_encrypted_msg, eklen);
  memcpy(iv, full_encrypted_msg + eklen, ivlen);
  enclen = len - eklen - ivlen;
  encMsg = (unsigned char *)malloc(enclen * sizeof(unsigned char *));
  memcpy(encMsg, full_encrypted_msg + eklen + ivlen, enclen);
  if ((declen = rsaDecrypt(encMsg, enclen, &ek[0], eklen, &iv[0], ivlen,
                           (unsigned char **)&decMsg)) == -1) {
    fprintf(stderr, "Decryption failed\n");
    return "";
  }
  decrypted_msg = string(reinterpret_cast<char *>(decMsg));
  return decrypted_msg;
}

int RSACrypto::encrypt(std::string msg) {
  if ((enclen = rsaEncrypt((const unsigned char *)msg.c_str(), msg.size() + 1,
                           &encMsg, &ek, &eklen, &iv, &ivlen)) == -1) {
    fprintf(stderr, "Encryption failed\n");
    return 1;
  }
  eklen = 256;
  full_encrypted_msg =
      (unsigned char *)malloc(sizeof(char) * (ivlen + eklen + enclen));
  memcpy(full_encrypted_msg, ek, eklen);
  memcpy(full_encrypted_msg + eklen, iv, ivlen);
  memcpy(full_encrypted_msg + eklen + ivlen, encMsg, enclen);
  full_encrypted_msg_len = eklen + ivlen + enclen;
  return 0;
}

int RSACrypto::save(std::string filename) {
  ofstream ofs;
  ofs.open(filename);
  ofs.write((char *)ek, eklen);
  ofs.write((char *)iv, ivlen);
  ofs.write((char *)encMsg, enclen);
  ofs.close();
  return 0;
}

/*
int Crypto::writeKeyToFile(FILE *fd, int key) {
    switch(key) {
        case KEY_SERVER_PRI:
            if(!PEM_write_PrivateKey(fd, localKeypair, NULL, NULL, 0, 0, NULL))
{
                return FAILURE;
            }
            break;

        case KEY_SERVER_PUB:
            if(!PEM_write_PUBKEY(fd, localKeypair)) {
                return FAILURE;
            }
            break;

        case KEY_CLIENT_PUB:
            if(!PEM_write_PUBKEY(fd, remotePubKey)) {
                return FAILURE;
            }
            break;
        default:
            return FAILURE;
    }

    return SUCCESS;
}
*/

int RSACrypto::getRemotePubKey(unsigned char **pubKey) {
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_PUBKEY(bio, remotePubKey);

  int pubKeyLen = BIO_pending(bio);
  *pubKey = (unsigned char *)malloc(pubKeyLen);
  if (pubKey == NULL) return FAILURE;

  BIO_read(bio, *pubKey, pubKeyLen);

  // Insert the NUL terminator
  (*pubKey)[pubKeyLen - 1] = '\0';

  BIO_free_all(bio);

  return pubKeyLen;
}

int RSACrypto::setRemotePubKey(unsigned char *pubKey, size_t pubKeyLen) {
  // BIO *bio = BIO_new(BIO_f_base64());
  BIO *bio = BIO_new(BIO_s_mem());
  if (BIO_write(bio, pubKey, pubKeyLen) != (int)pubKeyLen) {
    return FAILURE;
  }

  RSA *_pubKey = (RSA *)malloc(sizeof(RSA));
  if (_pubKey == NULL) return FAILURE;

  PEM_read_bio_PUBKEY(bio, &remotePubKey, NULL, NULL);

  BIO_free_all(bio);

  return SUCCESS;
}

/*
int Crypto::getLocalPubKey(unsigned char** pubKey) {
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, localKeypair);
    int pubKeyLen = BIO_pending(bio);
    *pubKey = (unsigned char*)malloc(pubKeyLen);
    if(pubKey == NULL) return FAILURE;
    BIO_read(bio, *pubKey, pubKeyLen);

    // Insert the NUL terminator
    (*pubKey)[pubKeyLen-1] = '\0';
    BIO_free_all(bio);
    return pubKeyLen;
}
*/

/*
int Crypto::getLocalPriKey(unsigned char **priKey) {
    BIO *bio = BIO_new(BIO_s_mem());

    PEM_write_bio_PrivateKey(bio, localKeypair, NULL, NULL, 0, 0, NULL);

    int priKeyLen = BIO_pending(bio);
    *priKey = (unsigned char*)malloc(priKeyLen + 1);
    if(priKey == NULL) return FAILURE;

    BIO_read(bio, *priKey, priKeyLen);

    // Insert the NUL terminator
    (*priKey)[priKeyLen] = '\0';

    BIO_free_all(bio);

    return priKeyLen;
}
*/

int RSACrypto::init() {
  // some const variables
  eklen = 256;  // 256 bytes
  ivlen = 16;   // 16 bytes initial vector for symmetric key
  enclen = 0;   // unknown
  decMsg = nullptr;
  encMsg = nullptr;

  // read key files
  if (pub_file_name != "") {
    if (readLocalPubKey(pub_file_name)) {
      cerr << "--->>> ERROR in read local pub key" << endl;
      return -1;
    }
  }

  if (pri_file_name != "") {
    if (readLocalPriKey(pri_file_name, pass)) {
      cerr << "--->>> ERROR in read local pri key" << endl;
      return -1;
    }
  }

  // Initalize contexts
  rsaEncryptCtx = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));

  rsaDecryptCtx = (EVP_CIPHER_CTX *)malloc(sizeof(EVP_CIPHER_CTX));

  // Always a good idea to check if malloc failed
  if (rsaEncryptCtx == NULL || rsaDecryptCtx == NULL) {
    return FAILURE;
  }

  // Init these here to make valgrind happy
  EVP_CIPHER_CTX_init(rsaEncryptCtx);

  EVP_CIPHER_CTX_init(rsaDecryptCtx);

  // Init RSA
  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

  if (EVP_PKEY_keygen_init(ctx) <= 0) {
    return FAILURE;
  }

  if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, RSA_KEYLEN) <= 0) {
    return FAILURE;
  }

  EVP_PKEY_CTX_free(ctx);

  return SUCCESS;
}

int RSACrypto::readLocalPubKey(std::string name) {
  FILE *keyfile = fopen(name.c_str(), "r");
  if (!keyfile) {
    std::cerr << "cannot open public file" << std::endl;
    return -1;
  }
  RSA *rsa_pub = PEM_read_RSA_PUBKEY(keyfile, NULL, NULL, NULL);
  if (!rsa_pub) {
    std::cerr << "cannot read rsa public key" << std::endl;
    return -1;
  }
  remotePubKey = EVP_PKEY_new();

  EVP_PKEY_assign_RSA(remotePubKey, rsa_pub);
  return 0;
}

int RSACrypto::readLocalPriKey(std::string name, std::string pass) {
  OpenSSL_add_all_algorithms();
  OpenSSL_add_all_ciphers();
  OpenSSL_add_all_digests();
  FILE *keyfile = fopen(name.c_str(), "r");
  if (!keyfile) {
    std::cerr << "cannot open private key, file=" << name << " pass=" << pass
              << std::endl;
    return -1;
  }
  localPriKey = EVP_PKEY_new();
  RSA *rsa_pkey = NULL;

  if (PEM_read_RSAPrivateKey(keyfile, &rsa_pkey, NULL, (void *)pass.c_str()) ==
      NULL) {
    ERR_print_errors_fp(stderr);
    return -1;
  }
  EVP_PKEY_assign_RSA(localPriKey, rsa_pkey);
  fclose(keyfile);
  return 0;
}
}
