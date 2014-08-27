/*
This is the file: int2string.cpp
Coder: Bin Fan
Email: fanbin.flash@gmail.com
Create Time: 2012/06/28 05:22:47
*/

#ifndef _bin_unixsystem_
#define _bin_unixsystem_ 1

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <fstream>
#include <random>
#include <algorithm>
#include <ctime>

using namespace std;
namespace bin {

inline std::streamsize FileLeftBytes(std::istream &is) {
  std::streampos pos = is.tellg();
  is.seekg(0, std::ios::end);
  std::streamsize len = is.tellg() - pos;
  is.seekg(pos);
  return len;
}

inline std::string DirName(const string &str) {
  size_t found;
  found = str.find_last_of("/\\");
  return str.substr(0, found);
}

inline bool isFileExist(const char *fileName) {
  std::ifstream infile(fileName);
  return infile.good();
}

inline bool isFileExist(const std::string fileName) {
  std::ifstream infile(fileName.c_str());
  return infile.good();
}

inline int LockFile(int fd) {
  struct flock fl;
  fl.l_type = F_WRLCK;
  fl.l_start = 0;
  fl.l_whence = SEEK_SET;
  fl.l_len = 0;
  return (fcntl(fd, F_SETLK, &fl));
}

inline std::string RandomString(size_t length) {
  static int round = 0;
  srand(time(NULL)+round);
  round++;
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

inline int already_running(std::string filename) {
  const char *LOCKMODE = "(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)";
  int fd;
  char buf[16];
  fd = open(filename.c_str(), O_RDWR | O_CREAT, LOCKMODE);
  if (fd < 0) {
    cerr << "--->>> System ERROR: cannot open lock file " << filename << endl;
    cerr << "--->>> System: another instance is running, current program exits"
         << endl;
    return 1;
  }

  if (LockFile(fd) < 0) {
    if (errno == EACCES || errno == EAGAIN) {
      close(fd);
      cerr
          << "--->>> System: another instance is running, current program exits"
          << endl;
      return 1;
    }
    cerr << "--->>> System ERROR: cannot lock file" << filename << endl;
    cerr << "--->>>System: another instance is running, current program exits"
         << endl;
    return 1;
  }
  ftruncate(fd, 0);
  sprintf(buf, "%ld", long(getpid()));
  write(fd, buf, strlen(buf) + 1);
  return 0;
}

//inline void demonize(const char *cmd) { umask(0); }
}

#endif
