//
//  data.h
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef stsdb_data_h
#define stsdb_data_h 1

#include <iostream>
#include <memory>
#include <map>
#include "hdf5.h"
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "TSHeader.h"


namespace stsdb {

//****************************DATA STRUCTURES*******************************

// **************** Base des Struct ***********************
typedef struct des {
  virtual size_t getNFIELDS() { return 1;}
  virtual const size_t* get_dst_offset() {return nullptr;};
  virtual const size_t* get_dst_sizes()  {return nullptr;};
  virtual const char** get_field_names()  {return nullptr;};
  virtual const hid_t* get_field_type() {return nullptr;};
} des;

//*******************************StockDailyRecord*****************************

struct _des_StockDailyRecord;
typedef _des_StockDailyRecord des_StockDailyRecord;


typedef struct {
  timestamp_t epoch;
  ieee32_t open;
  ieee32_t close;
  ieee32_t high;
  ieee32_t low;
  ieee32_t prev_close;
  ieee32_t vol;
  ieee64_t amt;
  static std::shared_ptr<des> getFeatures() {
    std::shared_ptr<des> ptr = std::static_pointer_cast<des>(std::make_shared<des_StockDailyRecord>());
    return ptr;
  }
} StockDailyRecord;


typedef struct _des_StockDailyRecord : public des {
  const  size_t NFIELDS = 8;
  size_t getNFIELDS() {return NFIELDS;}

  const size_t dst_offset[8] = {
      HOFFSET(StockDailyRecord, epoch), HOFFSET(StockDailyRecord, open),
      HOFFSET(StockDailyRecord, close), HOFFSET(StockDailyRecord, high),
      HOFFSET(StockDailyRecord, low),   HOFFSET(StockDailyRecord, prev_close),
      HOFFSET(StockDailyRecord, vol),   HOFFSET(StockDailyRecord, amt)};
  const size_t* get_dst_offset(){return dst_offset;}

  const size_t dst_sizes[8] = {
      sizeof(StockDailyRecord::epoch), sizeof(StockDailyRecord::open),
      sizeof(StockDailyRecord::close), sizeof(StockDailyRecord::high),
      sizeof(StockDailyRecord::low),   sizeof(StockDailyRecord::prev_close),
      sizeof(StockDailyRecord::vol),   sizeof(StockDailyRecord::amt)};
  const size_t* get_dst_sizes(){return dst_sizes;}

  const char* field_names[8] = {"TIMESTAMP", "open", "close", "high",
                                "low",       "prev", "vol",   "amt"};
  const char** get_field_names(){return field_names;}


  const hid_t field_type[8] = {H5T_STD_I64LE,  H5T_IEEE_F32LE, H5T_IEEE_F32LE,
                         H5T_IEEE_F32LE, H5T_IEEE_F32LE, H5T_IEEE_F32LE,
                         H5T_IEEE_F32LE, H5T_IEEE_F64LE};
  const hid_t* get_field_type() { return field_type;}

} des_StockDailyRecord;


typedef struct _StockHFRecord {
  timestamp_t epoch;
  ieee32_t price;
  int64_t vol;
  ieee64_t amt;
  size_t getNField() { return 4; }
} StockHFRecord;


// **************************** IndexDaily Record**********************
typedef struct _IndexDailyRecord {
  timestamp_t epoch;
  ieee32_t open;
  ieee32_t close;
  ieee32_t high;
  ieee32_t low;
  ieee32_t prev_close;
} IndexDailyRecord;

typedef struct _des_IndexDailyRecord : public des {
  const  size_t NFIELDS = 6;
  size_t getNFIELDS() {return NFIELDS;}

  const size_t dst_offset[6] = {
      HOFFSET(StockDailyRecord, epoch), HOFFSET(StockDailyRecord, open),
      HOFFSET(StockDailyRecord, close), HOFFSET(StockDailyRecord, high),
      HOFFSET(StockDailyRecord, low),   HOFFSET(StockDailyRecord, prev_close)};
  const size_t* get_dst_offset(){return dst_offset;}

  const size_t dst_sizes[6] = {
      sizeof(StockDailyRecord::epoch), sizeof(StockDailyRecord::open),
      sizeof(StockDailyRecord::close), sizeof(StockDailyRecord::high),
      sizeof(StockDailyRecord::low),   sizeof(StockDailyRecord::prev_close)};
  const size_t* get_dst_sizes(){return dst_sizes;}

  const char* field_names[6] = {"TIMESTAMP", "open", "close", "high",
                                "low",       "prev"};
  const char** get_field_names(){return field_names;}


  const hid_t field_type[6] = {H5T_STD_I64LE,  H5T_IEEE_F32LE, H5T_IEEE_F32LE,
                         H5T_IEEE_F32LE, H5T_IEEE_F32LE, H5T_IEEE_F32LE,
                         };
  const hid_t* get_field_type() { return field_type;}

} des_IndexDailyRecord;


typedef struct {
  timestamp_t epoch;
  ieee32_t price;
} IndexHFRecord;



typedef struct {
  timestamp_t epoch;
  ieee32_t settlement;
  ieee32_t close;
  ieee32_t open;
  ieee32_t high;
  ieee32_t low;
  ieee32_t prev_close;
  ieee32_t prev_settlement;
} FuturesDailyRecord;



typedef struct {
  timestamp_t epoch;
  ieee32_t price;
  ieee32_t bid1;
  ieee32_t ask1;
  ieee32_t bid2;
  ieee32_t ask2;
  ieee32_t bid3;
  ieee32_t ask3;
  int64_t vol;
  ieee64_t amt;
} FuturesHFRecord;

}
//******************************************************************
#endif
