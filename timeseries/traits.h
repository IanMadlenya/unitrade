//
//  traits.h
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef stsdb_Traits_h
#define stsdb_Traits_h

#include "ComHeader.h"
#include "data.h"

namespace stsdb {

///********************* type traits********************************
// level 1: distinguish stock, index, futures, option, bonds, swap, repo, ETF
// level 2: distinguish daily data and HF(high frequency) data
template <typename T>
struct is_DAILY {
  static bool const value = false;
};
template <>
struct is_DAILY<StockDailyRecord> {
  static const bool value = true;
};
template <>
struct is_DAILY<IndexDailyRecord> {
  static const bool value = true;
};
template <>
struct is_DAILY<FuturesDailyRecord> {
  static const bool value = true;
};

template <typename T>
struct is_HF {
  static bool const value = false;
};
template <>
struct is_HF<StockHFRecord> {
  static const bool value = true;
};
template <>
struct is_HF<IndexHFRecord> {
  static const bool value = true;
};
template <>
struct is_HF<FuturesHFRecord> {
  static const bool value = true;
};

template <typename T>
struct is_STOCK {
  static bool const value = false;
};
template <>
struct is_STOCK<StockHFRecord> {
  static const bool value = true;
};
template <>
struct is_STOCK<StockDailyRecord> {
  static const bool value = true;
};

template <typename T>
struct is_INDEX {
  static bool const value = false;
};
template <>
struct is_INDEX<IndexHFRecord> {
  static const bool value = true;
};
template <>
struct is_INDEX<IndexDailyRecord> {
  static const bool value = true;
};

template <typename T>
struct is_FUTURES {
  static bool const value = false;
};
template <>
struct is_FUTURES<FuturesHFRecord> {
  static const bool value = true;
};
template <>
struct is_FUTURES<FuturesDailyRecord> {
  static const bool value = true;
};

template <typename T>
SecurityType getSecurityType() {
  if (is_STOCK<T>::value)
    return SecurityType::STOCK;
  else if (is_INDEX<T>::value)
    return SecurityType::INDEX;
  else if (is_FUTURES<T>::value)
    return SecurityType::FUTURES;
};

template <typename T>
DataType getDataType() {
  if (is_HF<T>::value)
    return DataType::HF;
  else if (is_DAILY<T>::value)
    return DataType::DAILY;
};

///*****************************************************************

///*********************get typename********************************
// default implementation
template <typename T>
struct TypeName {
  static std::string Get() { return typeid(T).name(); }
};

// a specialization for each type of those you want to support
// and don't like the string returned by typeid
template <>
struct TypeName<StockDailyRecord> {
  static const std::string Get() { return "StockDailyRecord"; }
};

template <>
struct TypeName<IndexDailyRecord> {
  static const std::string Get() { return "IndexDailyRecord"; }
};
}
//******************************************************************
#endif
