//
//  HDFengine.h
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef __stsdb__record__
#define __stsdb__record__

#include <iostream>
#include <vector>
#include "hdf5.h"
#include "timeseries.h"
#include "hdf5_hl.h"

namespace stsdb {

int writeStringAttribution(const hid_t loc, const std::string att,
                           const std::string content);
int writeLongAttribution(const hid_t loc, const std::string att,
                         const long content);

inline int readStringAttribution(const hid_t loc, const std::string att,
                                 std::string &content, size_t len = 20) {
  auto attr = H5Aopen(loc, att.c_str(), H5P_DEFAULT);
  if (attr < 0) return -1;
  auto atype = H5Aget_type(attr);
  auto atype_mem = H5Tget_native_type(atype, H5T_DIR_ASCEND);
  if (atype_mem < 0) return -1;
  char *data = (char *)malloc((1 + len) * sizeof(char));
  auto ret = H5Aread(attr, atype_mem, data);
  if (ret < 0) return -1;
  content = std::string(data);
  free(data);
  H5Aclose(attr);
  H5Tclose(atype);

  return 0;
};

inline int readLongAttribution(const hid_t loc, const std::string att,
                               long &content) {
  auto attr = H5Aopen(loc, att.c_str(), H5P_DEFAULT);
  if (attr < 0) return -1;
  auto status = H5Aread(attr, H5T_NATIVE_LONG, &content);
  if (status < 0) return -1;
  H5Aclose(attr);
  return 0;
};

//long QueryLastRecordFromHDF(std::string name, std::string grouppath);


//------------- this is a function pointer that is to be transfered to H5Giterate
template <typename T>
herr_t create_read_file(hid_t loc_id, const char *code, void *opdata) {
  //std::cout<<code<<std::endl;  // for test, see if we successfully read data
  long num_in_file;
  std::string name;
  std::string gics;
  std::vector<TSPtr<T>> *ptr = (std::vector<TSPtr<T>> *)opdata;
  H5G_stat_t statbuf;
  H5Gget_objinfo(loc_id, code, false, &statbuf);
  auto _group = H5Gopen(loc_id, code, H5P_DEFAULT);

  if (!readLongAttribution(_group, "nrow", num_in_file) &&
      !readStringAttribution(_group, "name", name) &&
      !readStringAttribution(_group, "gics", gics)) {
    auto ts = std::make_shared<Timeseries<T>>(
                                              std::string(code),  // code
                                              name,     // data Chinese name
                                              "quote",  // dataset name
                                              gics      // gics
                                              );
    ptr->push_back(ts);
    std::vector<T> &data = ts->getDATA();
    auto new_data = new T[num_in_file];

    auto ptr_des = T::getFeatures();
    auto status =
        H5TBread_table(_group, "quote", sizeof(T), ptr_des->get_dst_offset(),
                       ptr_des->get_dst_sizes(), new_data);

    // always use insert instead of std::copy combined with back_insert
    data.insert(data.end(), &new_data[0], &new_data[num_in_file]);
    free(new_data);
    ts->Compile();

    return (int)status;
  } else {
    std::cerr << "--->>> stsdb: H5TBread_table " << code << " attribution is broken"
              << std::endl;
    return -1;
  }
};


template <typename T>
class HDFrecord {
 public:
  hid_t dataspace;
  hid_t datatype;
  HDFrecord(std::string file_name) {
    _security_type = getSecurityType<T>();
    _data_type = getDataType<T>();
    ptr_des = T::getFeatures();
    _type = stsdb::printSecurityType<T>();
    compress = true;
    chunk = 50;
    _file_name = file_name;
  }

  int Read(std::vector<TSPtr<T>> *ptr) {
    herr_t (*read_action)(hid_t loc_id, const char *name, void *opdata);
    read_action = &(create_read_file<T>);
    H5Eset_auto(H5E_DEFAULT, NULL,
                NULL);  // mute hdf error message, I will print it explicitly
    if (_file < 0) 
      return -1;  // file not opened

    _root_group = H5Gopen(_file, _type.c_str(), H5P_DEFAULT);
    if (_root_group < 0)  // if something wrong, root group cannot be opened                              
    {                                                                                                         
      std::cerr<<"ERROR: data section(root group) cannot be opened, data="<<_type<<std::endl;                                            
      H5Eprint(H5E_DEFAULT, stderr);                                                                          
      return -2;      
    }
    H5Giterate(_file, _type.c_str(), NULL, read_action, ptr);
    return 0;
  }

  int Init() {
    H5Eset_auto(H5E_DEFAULT, NULL, NULL);
    // try to open file
    _file = H5Fopen(_file_name.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    if (_file < 0)  // possibly the file does not exist, then create it
    {
      _file = H5Fcreate(_file_name.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT,
                        H5P_DEFAULT);
      if (_file < 0)  // something really wrong
      {
        H5Eprint(H5E_DEFAULT, stderr);
        return -1;
      }
    }
    // open group to read/store data
    std::string rgrp_path = "/"+_type;
    auto status = H5Gget_objinfo(_file, rgrp_path.c_str(), 0, NULL);
    if (status == 0)  // group exists, open it
    {
      _root_group = H5Gopen(_file, rgrp_path.c_str(), H5P_DEFAULT);
    } else  // group not exist, create it
    {
      _root_group = H5Gcreate(_file, rgrp_path.c_str(), H5P_DEFAULT, H5P_DEFAULT,
                         H5P_DEFAULT);
    }
    if (_root_group < 0)  // if something wrong, root group cannot be created
    {
      std::cerr<<"ERROR: root group cannot be created"<<std::endl;
      H5Eprint(H5E_DEFAULT, stderr);
      return -1;
    }
    return 0;
  };


  // write timeseries into database
  int Write(TSPtr<T> pt, bool overwrite = false) {
    auto NRECORDS = pt->getNRecords();
    _code = pt->getCODE();
    _name = pt->getNAME();
    _dataset = pt->getDATASET();
    
    auto first_ts = pt->getFIRSTTS();
    auto last_ts = pt->getLASTTS();
    auto gics = pt->getGICS();

    long num_in_file{}, first_in_file{}, last_in_file{};
    std::vector<T> &data = pt->getDATA();
    H5Eset_auto(H5E_DEFAULT, NULL,
                NULL);  // mute hdf error message, I will print it explicitly

    // check if the group exist
    auto status = H5Gget_objinfo(_root_group, _code.c_str(), 0, NULL);
    if (status == 0)  // group exists, open it
    {
      _group = H5Gopen(_root_group, _code.c_str(), H5P_DEFAULT);
      auto attr = H5Aopen(_group, "nrow", H5P_DEFAULT);
      status = H5Aread(attr, H5T_NATIVE_LONG, &num_in_file);
      attr = H5Aopen(_group, "first", H5P_DEFAULT);
      status = H5Aread(attr, H5T_NATIVE_LONG, &first_in_file);
      attr = H5Aopen(_group, "last", H5P_DEFAULT);
      status = H5Aread(attr, H5T_NATIVE_LONG, &last_in_file);
      is_group_new = false;
      if (last_in_file >= last_ts) {
        H5Aclose(attr);
        H5Gclose(_group);
        return 1;
      }
    } else  // group not exist, create it
    {
      _group = H5Gcreate(_root_group, _code.c_str(), H5P_DEFAULT, H5P_DEFAULT,
                         H5P_DEFAULT);
      first_in_file = first_ts;
    }
    if (_group < 0)  // if something wrong happens
    {
      std::cerr<<"ERROR: data group cannot be created or read; code="<<_code<<std::endl;
      H5Eprint(H5E_DEFAULT, stderr);
      return -1;
    }

    if (_security_type == SecurityType::STOCK && _data_type == DataType::DAILY) {
      hsize_t nfields_out;
      hsize_t nrecords_out;
      status = H5TBget_table_info(_group, _dataset.c_str(), &nfields_out,
                                  &nrecords_out);



      if (status < 0)  // table not exist
        status = H5TBmake_table(
            _dataset.c_str(), _group, _dataset.c_str(), ptr_des->getNFIELDS(),
            NRECORDS, sizeof(T), ptr_des->get_field_names(),
            ptr_des->get_dst_offset(), ptr_des->get_field_type(), chunk, NULL,
            compress, data.data());
      else  // table already exists
      {
        if (data.front().epoch > last_ts)  // append data if no overlap
          status =
              H5TBappend_records(_group, _dataset.c_str(), NRECORDS, sizeof(T),
                                 ptr_des->get_dst_offset(), ptr_des->get_dst_sizes(), data.data());
        else if (!overwrite) {
          auto resume_start = std::find_if(
              data.begin(), data.end(),
              [last_in_file](T const &t) { return t.epoch > last_in_file; });
          NRECORDS = distance(resume_start, data.end());
          status = H5TBappend_records(_group, _dataset.c_str(), NRECORDS,
                                      sizeof(T), ptr_des->get_dst_offset(), ptr_des->get_dst_sizes(),
                                      &(*resume_start));
        }
      }

      num_in_file += NRECORDS;
      last_in_file = last_ts;
    }

    if (writeStringAttribution(_group, "name", _name)) {
      throw TimeseriesException("HDF attribution write error");
    }
    if (writeStringAttribution(_group, "code", _code)) {
      throw TimeseriesException("HDF attribution write error");
    }
    if (writeStringAttribution(_group, "beg", printTime(first_in_file))) {
      throw TimeseriesException("HDF attribution write error");
    }
    if (writeStringAttribution(_group, "end", printTime(last_in_file))) {
      throw TimeseriesException("HDF attribution write error");
    }
    if (writeStringAttribution(_group, "gics", gics)) {
      throw TimeseriesException("HDF attribution write error");
    }
    if (writeLongAttribution(_group, "nrow", num_in_file)) {
      throw TimeseriesException("HDF attribution write error");
    }
    if (writeLongAttribution(_group, "last", last_in_file)) {
      throw TimeseriesException("HDF attribution write error");
    }
    if (writeLongAttribution(_group, "first", first_in_file)) {
      throw TimeseriesException("HDF attribution write error");
    }

    // close handlers
    H5Gclose(_group);
    return 0;
  }

  int finalize() {
    if (_file > 0) H5Fclose(_file);
    return 0;
  }

 private:
  std::shared_ptr<des> ptr_des;
  SecurityType _security_type;
  DataType _data_type;
  std::string _code;
  std::string _name;
  std::string _dataset;
  std::string _grppath;
  std::string _type;
  hid_t _root_group, _group, _file;
  std::string _file_name;
  bool compress;
  size_t chunk;
  bool is_group_new{false};
};

}  // namespace stsdb

#endif /* defined(__stsdb__record__) */
