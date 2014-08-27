/*
This is the file: int2string.cpp
Coder: Bin Fan
Email: fanbin.flash@gmail.com
Create Time: 2012/06/28 05:22:47
*/

#ifndef _bin_h_int2string_
#define _bin_h_int2string_ 1
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <type_traits>

namespace bin {

template <typename T>
inline T string_to(const std::string& str) {
  T var;
  std::istringstream iss;
  iss.str(str);
  iss >> var;
  // deal with any error bits that may have been set on the stream
  return var;
};

inline std::string int2string(int target) {
  return std::to_string(target);
}

inline std::string float2string(float target, int precision = 2) {
  return std::to_string(target);
}

inline std::string double2string(double target, int precision = 2) {
  return std::to_string(target);
}

inline int string2int(std::string str) { return atoi(str.c_str()); }

inline double string2double(std::string str) { return atof(str.c_str()); }

inline float string2float(std::string str) { return atof(str.c_str()); }
}

#endif
