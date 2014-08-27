
#ifndef _bin_ssl_conversation_
#define _bin_ssl_conversation_ 1

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <ctime>
#include <cstring>
#include <string>
#include <iostream>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

using namespace std;

namespace binsec {
//------------Unix Inter Program Communication (Domain socket) ---------

 

  class SSLStream {
 private:
  std::string name{};
  unsigned long _port{};
  std::string _domain{};
  std::string _crt{};
  std::string _key{};
  std::string _msg{};
  std::string _ans{};

  SSL_CTX *ctx;
  X509 *cert;
  SSL *ssl;
  int fd, cl;
  bool silent;
 public:
  SSLStream(std::string const nm="", bool s = false) {
    name = nm;
    silent = s;
  }
  bool isDisConnected {false};

  int silence(bool t) {
    silent = t;
    return 0;
  }
  void ShowCert(SSL *ssl);
  int stream_server_setup(const int port, const std::string crt,
                          const std::string key);
  int stream_client_setup(const int port, const std::string hostname);
  int InitCTX();
  int LoadCertificates(SSL_CTX *ctx, const char *CertFile, const char *KeyFile);
  int server_start();
  int stream_accept();
  int stream_send_key(unsigned char *m, size_t len);
  size_t stream_get_key(unsigned char* &p);

  std::string stream_read();
  int stream_answer(std::string const &m);
  int stream_clean();
  int stream_process();
  //--for client
  int stream_connect();
  int stream_disconnect();
  int stream_ask(std::string const &m, std::string &ans);
  ~SSLStream()
  {
    if (ctx)
      SSL_CTX_free(ctx);
  }
};

inline void ShowCerts(SSL *ssl) {
  X509 *cert;
  char *line;
  cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
  if (cert != NULL) {
    printf("Server certificates:\n");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line); /* free the malloc'ed string */
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);      /* free the malloc'ed string */
    X509_free(cert); /* free the malloc'ed certificate copy */
  } else
    printf("No certificates.\n");
}

inline int SSLStream::stream_process() {
  if (_msg == "shutdown") {
    stream_clean();
  }
  return 0;
}

inline int SSLStream::stream_clean() {
  if (ssl) SSL_free(ssl);
  if (fd) close(fd);
  if (ctx) SSL_CTX_free(ctx);
  ctx = nullptr;
  return 0;
}

inline int SSLStream::InitCTX() {
  // init ctx
  signal(SIGPIPE, SIG_IGN);
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  ctx = SSL_CTX_new(TLSv1_method());
  if (ctx == NULL) {
    ERR_print_errors_fp(stderr);
    return -1;
  }
  return 0;
};

inline int SSLStream::LoadCertificates(SSL_CTX *ctx, const char *CertFile,
                                       const char *KeyFile) {
  if (SSL_CTX_load_verify_locations(ctx, CertFile, KeyFile) != 1)
    ERR_print_errors_fp(stderr);

  if (SSL_CTX_set_default_verify_paths(ctx) != 1) ERR_print_errors_fp(stderr);

  if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    abort();
  }

  /* set the private key from KeyFile (may be the same as CertFile) */
  if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    abort();
  }

  /* verify private key */
  if (!SSL_CTX_check_private_key(ctx)) {
    fprintf(stderr, "Private key does not match the public certificate\n");
    abort();
  }
  printf("LoadCertificates complete successfully.....\n");
  return 0;
}

inline int SSLStream::stream_server_setup(const int port, const std::string crt,
                                          const std::string key) {
  _port = port;
  _crt = crt;
  _key = key;

  SSL_library_init();
  InitCTX();
  LoadCertificates(ctx, crt.c_str(), key.c_str());
  return 0;
};

inline int SSLStream::stream_client_setup(const int port,
                                          const std::string hostname) {
  _port = port;
  _domain = hostname;
  SSL_library_init();
  InitCTX();
  return 0;
};

inline int SSLStream::server_start() {
  struct sockaddr_in addr;
  // char buf[1000];
  // int rc;
signal(SIGPIPE, SIG_IGN);
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }
  if (!silent) cerr << "--->>> " << name << ": socket constructed" << endl;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (::bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }
  if (!silent) cerr << "--->>> " << name << ": socket binding succeeds" << endl;
  if (listen(fd, 5000) == -1) {
    perror("listen error");
    exit(-1);
  }
  if (!silent) cerr << "--->>> " << name << ": listen pipe constructed" << endl;
  return 0;
};

inline int SSLStream::stream_accept() {
  if (!silent)
    cerr << "--->>> " << name << ": systerm ready, waiting for connection..."
         << endl;
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  ssl = nullptr;

  cl = accept(fd, (struct sockaddr *)&addr, &len);
  printf("--->>> Server: connection: %s:%d\n", inet_ntoa(addr.sin_addr),
         ntohs(addr.sin_port));
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, cl); /* set socket to SSL state*/

  auto status = SSL_accept(ssl);
  if (status != -1) {
    ERR_print_errors_fp(stderr);
  }
  if (!silent) cerr << "--->>> " << name << ": connection linked" << endl;

  return 0;
};

inline std::string SSLStream::stream_read() {
  // long rc;
  char buf[1024];

  auto bytes = SSL_read(ssl, buf, sizeof(buf));

  if (bytes > 0) {
    buf[bytes] = '\0';
    if (!silent) cerr << "--->>> " << name << ": message rec" << endl;
    _msg = std::string(buf);
  } 
  else
  {
    ERR_print_errors_fp(stderr);
    isDisConnected = true;
  }
  return _msg;
};

inline size_t SSLStream::stream_get_key(unsigned char* &buf) {
  // long rc;
  if (buf)
    free(buf);
  buf = (unsigned char *)malloc(2049 * sizeof(char));
  auto len = SSL_read(ssl, buf, 2048);
  if (len > 0) {
    buf[len] = '\0';
    if (!silent) cerr << "--->>> " << name << ": key message rec" << endl;

  } else
  {
    ERR_print_errors_fp(stderr);
    isDisConnected = true;
  }
  return len;
};

inline int SSLStream::stream_connect() {
  struct hostent *host;
  struct sockaddr_in addr;
  // int rc;
  // initiate socket
  //
  ssl = nullptr;

  if ((host = gethostbyname(_domain.c_str())) == NULL) {
    std::cerr << "Error: " << _domain << std::endl;
    perror(_domain.c_str());
    return -1;
  }

  if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(_port);
  addr.sin_addr.s_addr = *(long *)(host->h_addr);

  // connect to socket
  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    close(fd);
    return -1;
  }
  ssl = SSL_new(ctx);
  SSL_set_fd(ssl, fd);
  if (SSL_connect(ssl) == -1) {
    // printf('Eroor: %s\n',stderr);
    ERR_print_errors_fp(stderr);
    return -1;
  }
  ShowCerts(ssl);
  if (!silent) cerr << "--->>> " << name << ": connected to server" << endl;
  // waiting to receive information
  //SSL_free(ssl);
  isDisConnected = false;
  return 0;
};

inline int SSLStream::stream_disconnect() {
  if (ssl) {
    auto sd = SSL_get_fd(ssl);
    SSL_free(ssl);
    close(sd);
  }
  isDisConnected = true;
  return 0;
};

inline int SSLStream::stream_ask(std::string const &m, std::string &ans) {
  long rc;
  // char buf[1000];
  signal(SIGPIPE, SIG_IGN);
  char answer[1000];
  char *message = const_cast<char *>(m.c_str());
  rc = SSL_write(ssl, message, strlen(message) + 1);
  if (rc != (long)strlen(message) + 1) {
    if (rc > 0)
      fprintf(stderr, "partial write");
    else {
      perror("write error");
      return -1;
    }
  }
  if (!silent) cerr << "--->>> " << name << ": request sent" << endl;
  rc = SSL_read(ssl, answer, sizeof(answer));
  // buf[rc]=0
  if (rc<=0)
  {
    isDisConnected = true;
    return -2;
  }
  if (!silent) cerr << "--->>> " << name << ": response rec" << endl;
  ans = string(answer);
  return 0;
};

inline int SSLStream::stream_send_key(unsigned char *m, size_t len) {
  signal(SIGPIPE, SIG_IGN);
  auto rc = SSL_write(ssl, m, len);
  if ((size_t)rc != len) {
    if (rc > 0)
    {
      fprintf(stderr, "partial write");
      return -2;
    }
    else 
    {
      perror("write error");
      return -1;
    }
  }
  if (!silent) cerr << "--->>> " << name << ": stream sends key" << endl;
  return 0;
};

inline int SSLStream::stream_answer(std::string const &m) {
  long rc;
  // char buf[1000];
   signal(SIGPIPE, SIG_IGN); 
  char *message = const_cast<char *>(m.c_str());
  rc = SSL_write(ssl, message, strlen(message) + 1);
  if (rc != (long)strlen(message) + 1) {
    if (rc > 0)
    {
      fprintf(stderr, "partial write");
      isDisConnected = true;
      return -2;
    }
    else {
      perror("write error");
      isDisConnected = true;
      return -1;
    }
  }
  if (!silent) cerr << "--->>> " << name << ": answer sent" << endl;
  return 0;
};
}

#endif

