#ifndef _bin_timeseries_util
#define _bin_timeseries_util 1

//
//  timeseries_util.h
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#include "timeseries.h"

namespace stsdb {

inline timestamp_t ptime_to_timestamp(ptime timepoint) {
  auto duration = timepoint.time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
};

inline ptime timestamp_to_ptime(timestamp_t timestamp) {
  std::chrono::duration<long, std::milli> du(timestamp);
  ptime tp(du);
  return tp;
}

inline std::string printTime(ptime timepoint) {
  std::time_t time_c = std::chrono::system_clock::to_time_t(timepoint);
  char mbstr[100];
  if (std::strftime(mbstr, sizeof(mbstr), "%Y%m%d.%H:%M:%S",
                    std::localtime(&time_c))) {
    return std::string(mbstr);
  }
  return "";
}

inline std::string printTime(timestamp_t timestamp) {
  std::time_t time_c = timestamp/1000;
  char mbstr[100];
  if (std::strftime(mbstr, sizeof(mbstr), "%Y%m%d.%H:%M:%S",
                    std::localtime(&time_c))) {
    return std::string(mbstr);
  }
  return "";
}

inline ptime parseTime(std::string timestring, std::string format) {
  std::tm tm = {0};
  strptime(timestring.c_str(), format.c_str(), &tm);
  ptime tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::from_time_t(std::mktime(&tm)));
  return tp;
}

inline timestamp_t parseTimeToStamp(std::string timestring, std::string format) {
  auto tp = parseTime(timestring, format);
  return ptime_to_timestamp(tp);
}
}

#endif
