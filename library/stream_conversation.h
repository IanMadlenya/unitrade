#ifndef _bin_stream_conversation_
#define _bin_stream_conversation_ 1

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <ctime>
#include <cstring>
#include <string>
#include <iostream>
#include <string>

using namespace std;

namespace bin
{
//------------Unix Inter Program Communication (Domain socket) ---------
class DomainStream {
 private:
  std::string name;
  char *socket_path;
  int fd, cl;
  bool silent;

 public:
  DomainStream(std::string const n, std::string const  sock, bool s = false) {
    name = n;
    socket_path = const_cast<char *>(sock.c_str());
    silent = s;
  }
 
  int silence(bool t) {
    silent = t;
    return 0;
  }
  int stream_setup();
  int stream_accept();
  int stream_read(std::string &msg);
  int stream_answer(std::string const &m);
  //--for client
  int stream_connect();
  int stream_disconnect();
  int stream_ask(std::string const &m, std::string &ans);
};


inline int DomainStream::stream_setup() 
{
  struct sockaddr_un addr;
  //char buf[1000];
  //int rc;

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }
  if (!silent)
    cerr<<"--->>> "<<name<<": socket constructed"<<endl;

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  unlink(socket_path);

  if (::bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }
  if (!silent)
    cerr<<"--->>> "<<name<<": socket binding succeeds"<<endl;
  if (listen(fd, 5000) == -1) {
    perror("listen error");
    exit(-1);
  }
  if (!silent)
    cerr<<"--->>> "<<name<<": listen pipe constructed"<<endl;
  return 0;
};

inline int DomainStream::stream_accept()
{
  if (!silent)
    cerr<<"--->>> "<<name<<": systerm ready, waiting for connection..."<<endl;
  if ( (cl = accept(fd, NULL, NULL)) == -1) {
      perror("accept error");
  }
  if (!silent)
    cerr<<"--->>> "<<name<<": connection linked"<<endl;
  return 0;
};

inline int DomainStream::stream_read(string &msg)
{
  long rc;
  char buf[1000];

  if ( (rc=read(cl,buf,sizeof(buf))) > 0) 
  {
      if (!silent)
        cerr<<"--->>> "<<name<<": message rec"<<endl;
      msg = std::string(buf);
  }
  else if (rc == -1) 
  {
    perror("read");
    exit(-1);
  }
  else if (rc == 0) 
  {
    cerr<<"--->>> "<<name<<": transimission terminated"<<endl;
  }
  return 0;
};


inline int DomainStream::stream_connect() 
{
  struct sockaddr_un addr;
  //int rc;
  // initiate socket
  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  // connect to socket
  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }
  if (!silent)
    cerr<<"--->>> "<<name<<": connected to server"<<endl;
  // waiting to receive information

  return 0;
};

inline int DomainStream::stream_disconnect() 
{
  close(cl);
  return 0;
};


inline int DomainStream::stream_ask(std::string const &m, std::string &ans) 
{
  long rc;
  //char buf[1000];
  char answer[1000];
  char *message = const_cast<char*>(m.c_str());
  rc = write(fd, message, strlen(message)+1);
  if ( rc != (long)strlen(message)+1) 
  {
      if (rc > 0) fprintf(stderr,"partial write");
      else 
      {
        perror("write error");
        exit(-1);
      }
  }
  if (!silent)
    cerr<<"--->>> "<<name<<": request sent"<<endl;
  rc=read(fd,answer,sizeof(answer));
  if (!silent)
    cerr<<"--->>> "<<name<<": response rec"<<endl;
  ans=string(answer); 
  return 0;
};



inline int DomainStream::stream_answer(std::string const &m) 
{
  long rc;
  //char buf[1000];
  char *message = const_cast<char*>(m.c_str());
  rc = write(cl, message, strlen(message)+1);
  if ( rc != (long)strlen(message)+1) 
  {
      if (rc > 0) fprintf(stderr,"partial write");
      else 
      {
        perror("write error");
        exit(-1);
      }
  }
  if (!silent)
    cerr<<"--->>> "<<name<<": answer sent"<<endl;
  return 0;
};

}

#endif
