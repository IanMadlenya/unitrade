//
//  HDFengine.cpp
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#include "HDFengine.h"
namespace stsdb {
int writeStringAttribution(hid_t loc, std::string att, std::string content) {
  // check if attributes are correctly set
  hid_t attr;
  hid_t atype = H5Tcopy(H5T_C_S1);
  H5Tset_size(atype, content.size() + 1);
  H5Tset_strpad(atype, H5T_STR_NULLTERM);
  hid_t status = H5Aexists(loc, att.c_str());
  if (status != 0)  // it does not exist, create it
    attr = H5Aopen(loc, att.c_str(), H5P_DEFAULT);
  else
    attr = H5Acreate2(loc, att.c_str(), atype, H5Screate(H5S_SCALAR),
                      H5P_DEFAULT, H5P_DEFAULT);

  if (attr < 0)  // something really wrong
  {
    H5Eprint(H5E_DEFAULT, stderr);
    // throw TimeseriesException("HDF attribution open error");
    return -1;
  }

  // write attribution
  status = H5Awrite(attr, atype, content.c_str());
  if (status != 0)  // something really wrong
  {
    H5Eprint(H5E_DEFAULT, stderr);
    // throw TimeseriesException("HDF attribution write error");
    return -1;
  }
  H5Aclose(attr);
  return 0;
};

int writeTimeStampAttribution(hid_t loc, std::string att, long content) {
  // check if attributes are correctly set
  hid_t attr;
  hid_t status = H5Aexists(loc, att.c_str());
  if (status != 0)  // it does not exist, create it
    attr = H5Aopen(loc, att.c_str(), H5P_DEFAULT);
  else
    attr = H5Acreate(loc, att.c_str(), H5T_NATIVE_LONG, H5Screate(H5S_SCALAR),
                     H5P_DEFAULT, H5P_DEFAULT);
  if (attr < 0)  // something really wrong
  {
    H5Eprint(H5E_DEFAULT, stderr);
    return -1;
  }

  // write attribution
  status = H5Awrite(attr, H5T_NATIVE_LONG, &content);
  if (status != 0)  // something really wrong
  {
    H5Eprint(H5E_DEFAULT, stderr);
    return -1;
  }
  H5Aclose(attr);
  return 0;
};

int writeLongAttribution(hid_t loc, std::string att, long content) {
  // check if attributes are correctly set
  hid_t attr;

  hid_t status = H5Aexists(loc, att.c_str());
  if (status != 0)  // it does not exist, create it
    attr = H5Aopen(loc, att.c_str(), H5P_DEFAULT);
  else
    attr = H5Acreate(loc, att.c_str(), H5T_NATIVE_LONG, H5Screate(H5S_SCALAR),
                     H5P_DEFAULT, H5P_DEFAULT);
  if (attr < 0)  // something really wrong
  {
    H5Eprint(H5E_DEFAULT, stderr);
    return -1;
  }

  // write attribution
  status = H5Awrite(attr, H5T_NATIVE_LONG, &content);
  if (status != 0)  // something is really wrong
  {
    H5Eprint(H5E_DEFAULT, stderr);
    return -1;
  }
  H5Aclose(attr);
  return 0;
};

long QueryLastRecordFromHDF(std::string name, std::string grouppath) {
  hid_t group;
  // mute hdf error message, I will print it explicitly if necessary
  H5Eset_auto(H5E_DEFAULT, NULL, NULL);
  // try open file
  // std::cout<<name<<"\t"<<grouppath<<std::endl;
  auto file = H5Fopen(name.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  if (file < 0)  // possibly the file does not exist
    return -1;
  auto status = H5Gget_objinfo(file, grouppath.c_str(), 0, NULL);
  if (status == 0)  // group exists, open it
    group = H5Gopen(file, grouppath.c_str(), H5P_DEFAULT);
  else  // group not exist, create it
    return -1;

  long last;
  auto attr = H5Aopen(group, "last", H5P_DEFAULT);

  status = H5Aread(attr, H5T_NATIVE_LONG, &last);

  H5Aclose(attr);
  H5Gclose(group);
  H5Fclose(file);
  return last;
};

std::istream& operator>>(std::istream& str, CSVRow& data) {
  data.readNextRow(str);
  return str;
};
}

