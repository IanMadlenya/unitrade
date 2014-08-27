//
//  TSHeader.h
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef stsdb_ComHeader_h
#define stsdb_ComHeader_h

#include <iostream>
#include <memory>
#include <map>
#include "hdf5.h"
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cstring>
//#include "ComHeader.h"

namespace stsdb {

class CSVRow {
 public:
  std::string const& operator[](std::size_t index) const {
    return m_data[index];
  }
  std::size_t size() const { return m_data.size(); }
  void readNextRow(std::istream& str) {
    std::string line;
    std::getline(str, line);

    std::istringstream lineStream(line);
    std::string cell;

    m_data.clear();
    while (std::getline(lineStream, cell, ',')) {
      m_data.push_back(cell);
    }
  }

 private:
  std::vector<std::string> m_data;
};

typedef long int32_t;
typedef double ieee64_t;
typedef float ieee32_t;
typedef signed char int8_t;
typedef char char_t;
typedef long long timestamp_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned long long record_t;
typedef long date_t;
typedef std::chrono::time_point<std::chrono::system_clock,
                                std::chrono::milliseconds> ptime;

timestamp_t ptime_to_timestamp(ptime timepoint);
ptime timestamp_to_ptime(timestamp_t timestamp);
std::string printTime(const ptime pt);
std::string printTime(const timestamp_t pt);
ptime parseTime(const std::string timestring, const std::string format);
timestamp_t parseTimeToStamp(std::string timestring, std::string format);
std::istream& operator>>(std::istream& str, CSVRow& data);

//******************************enums**************************
enum DataType { DAILY, HF };
enum SecurityType { STOCK, INDEX, FUTURES, ETF, BOND, SWAP, REPO,OPTION};
//*************************************************************

//************** expception class***********************
class TimeseriesException : public std::runtime_error {
 public:
  TimeseriesException(const std::string& what)
      : std::runtime_error(std::string("SecurityTimeseriesException: ") +
                           what) {}
};

}
//******************************************************************
#endif
