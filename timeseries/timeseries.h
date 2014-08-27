//
//  timeseries.h
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef __stsdb__timeseries__
#define __stsdb__timeseries__

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include "hdf5.h"
#include "ComHeader.h"
#include "traits.h"
#include "data.h"
#include "timeseries_util.h"


using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

namespace stsdb {

// foward declaration
template <typename T>
class Timeseries;


// c++11 used only, template alias
template <typename T>
using TSPtr = std::shared_ptr<Timeseries<T>>;  // <-- C++0x

// factory to create timeseries instance

template <typename T>
class Timeseries {
 public:
 

  /* Constructors */
  Timeseries(  // type of security stock/index/futures/option
             std::string cd,   // security code, will be used as group name
             std::string nm,   // security name, will be used as group attribute
             std::string dn,   // dataset name, will be used as dataset name, like quote 
             std::string gics) {

    _code = cd; 
    _name = nm;
    _dataname = dn; 
    chunk = 50;  // make chunk size as around 2 months--except for HF data
    _security_type = getSecurityType<T>();
    _data_type = getDataType<T>();
    data.reserve(chunk);
    _gics = gics;
  }

  
  // friend int SetTimeSeriesType(std::shared_ptr<Timeseries> series,
  // SecurityType type);
  // friend std::shared_ptr<Timeseries> TimeseriesFactory(std::string code,
  // std::string name, SecurityType type, std::string dataset, std::string file)

  Timeseries(std::string readsource, std::string cd) {
    _code = cd;
    _size = 0;
    data.reserve(chunk);
  };

  /* Methods to lookup records in the Timeseries */
  herr_t recordId_LE(timestamp_t timestamp, hsize_t* record_id);
  herr_t recordId_GE(timestamp_t timestamp, hsize_t* record_id);
  herr_t recordId_LE(ptime timestamp, hsize_t* record_id);
  herr_t recordId_GE(ptime timestamp, hsize_t* record_id);
  void* getLastRecord(void);
  void* getRecordsById(hsize_t first, hsize_t last);
  hsize_t getNRecordsByTimestamp(ptime start, ptime end);
  hsize_t getNRecordsByTimestamp(timestamp_t start, timestamp_t end);
  void getRecordsByTimestamp(ptime start, ptime end, hsize_t* nrecords,
                             void** records);
  void getRecordsByTimestamp(timestamp_t start, timestamp_t end,
                             hsize_t* nrecords, void** records);

  int appendRecord(std::vector<T> record, bool is_overwrite = true) {
    // check if the passed in record is OK
    if (record.empty())  // if empty, just return
      return 0;
    if (sizeof(record[0]) != sizeof(T))  // passed in record is not compatible
      return -1;
    if (record[0].epoch >
        _last_ts)  // if the fist record timestamp is behind our record
      data.insert(data.end(), record.begin(), record.end());
    else if (is_overwrite) {
      // there must be some overlap, overwrite shall exercise

      // find from where shall we overwrite
      auto overwrite_mark = record[0].epoch;

      auto overwrite_start = std::find_if(
          data.begin(), data.end(),
          [overwrite_mark](T const& t) { return t.epoch >= overwrite_mark; });
      // then overwrite the data vector
      data.erase(overwrite_start, data.end());
      data.reserve(data.size() + record.size() + 1);
      data.insert(data.end(), record.begin(), record.end());
    } else if (!is_overwrite) {
      // if not overwrite, then we shall drop some new data
      // find from where shall we overwrite
      auto drop_mark = data.back().epoch;
      auto drop_start =
          std::find_if(record.begin(), record.end(),
                       [drop_mark](T const& t) { return t.epoch > drop_mark; });
      if (drop_start != record.end())
        data.insert(data.end(), drop_start, record.end());
    }

    // update relevant paras
    _first_ts = data.front().epoch;
    _last_ts = data.back().epoch;

    return 0;
  }

  /* Methods to get information about the Timeseries */
  hsize_t getNRecords(void) const {return data.size();};
  std::string getGICS(void) const {return _gics;}
  std::string getCODE(void) const {return _code;}
  std::string getNAME(void) const {return _name;}
  std::string getDATASET(void) const {return _dataname;}
  void Compile() {_first_ts = data.front().epoch; _last_ts = data.back().epoch;} 
  timestamp_t getFIRSTTS() {return _first_ts;}
  timestamp_t getLASTTS()  {return _last_ts;}
  std::vector<T>& getDATA() {return data;}

 private:
  std::vector<T> data;
  int chunk {};
  long _size {};
  std::string _code;
  std::string _name;
  std::string _gics;
  std::string _dataname;

  SecurityType _security_type;
  DataType _data_type;

  /* Private methods to deal with the Timeseries' index */
  bool createIndexIfNecessary(void);
  void indexTail(void);
  /* Properties */

  timestamp_t _first_ts {};
  timestamp_t _last_ts {};
 
};
}

#endif
