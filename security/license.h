#ifndef _license_H_
#define _license_H_
#include "rsacrypto.h"
#include "aescrypto.h"

#include <string>
#include <iostream>
//#include <openssl/rsa.h>
//#include <openssl/sha.h>
//#include <openssl/pem.h>
#include <string>
#include <iostream>
#include <stdint.h>
#include <chrono>
#include <ctime>
#include "bin.h"
#include "security.h"

using namespace std;

namespace binsec {

class Meta {
  public:
    std::string hostname;
    std::string ip;
    std::string user;
  Meta(){
    
  }
};


class License {
 public:
  std::string license_file_name;
  License(std::string filename = "") {
    license_file_name = filename;
    aescrypto.Init(binsec::GetUniqueMachineCode(), license_file_name);
  }
  int Load(std::string filename = "") {
    if (filename != "") {
      license_file_name = filename;
    }
    if (!bin::isFileExist(license_file_name)) return -1;
    aescrypto.Load(license_file_name);
    //time_to_expire = atoi(aescrypto.decrypt().c_str());
    ParseLicense(aescrypto.decrypt());
    return 0;
  }
  int Save(std::string filename = "") {
    if (filename != "") {
      license_file_name = filename;
    }
    aescrypto.encrypt(CompileLicense());
   
    //aescrypto.encrypt(std::to_string(time_to_expire));
    aescrypto.Save(license_file_name);
    return 0;
  }
  int time_to_expire{};
  bin::Date created;
  bin::Date last_modified;
  int ParseLicense(std::string msg)
  {
    auto ret = bin::ParseTokenFromString(msg); 
    auto it  = ret.find("toexpire");
    if (it!=ret.end())
      time_to_expire = atoi(it->second.c_str());
    if ( (it=ret.find("created")) !=ret.end())
      created.Set(it->second);
    if ( (it=ret.find("last_modified")) !=ret.end())
      last_modified.Set(it->second);

    return 0;
  }
  std::string CompileLicense()
  {
    std::string ret;
    ret += "toexpire="+std::to_string(time_to_expire)+"&";
    ret += "created="+ printDate(created)+"&";
    ret += "lastmodified="+printDate(last_modified);
    return ret;
  }
  binsec::AesCrypto aescrypto;

};

}

#endif
