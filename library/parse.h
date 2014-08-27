#ifndef _bin_h_parse_
#define _bin_h_parse_ 1

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "bin.h"
#include <sstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <locale>
#include <cctype>

namespace bin {

// check if file is empty
inline bool isFileEmpty(std::ifstream &pFile) {
  return pFile.peek() == std::ifstream::traits_type::eof();
}

inline bool isFileEmpty(const std::string &filename) {
  std::ifstream ifs;
  ifs.open(filename.c_str());
  if (!ifs.is_open())
  {
    return true;
  }
  else
    return ifs.peek() == std::ifstream::traits_type::eof();
}


// left trim string
inline std::string &ltrim(std::string &s) {
  if (s.size()==0)
    return s;
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char s) {
                       return !std::isspace<char>(s, std::locale::classic());
                     }));
  return s;
}

// right tring string
inline std::string &rtrim(std::string &s) {
  if (s.size()==0)
    return s; 
  s.erase(std::find_if(s.rbegin(), s.rend(), [](char s) {
                         return !std::isspace<char>(s, std::locale::classic());
                       }).base(),
          s.end());
  return s;
}

inline std::string &trim(std::string &s) { return ltrim(rtrim(s)); }



inline std::vector<std::string> SplitString(const std::string &s, char delim){
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
};


// split line into vector of strings
inline std::vector<std::string> SplitLine(const std::string line) {
  std::stringstream ss;
  std::istream_iterator<std::string> begin, end;
  std::vector<std::string> vstrings;
  ss << line;
  begin = std::istream_iterator<std::string>(ss);
  vstrings = std::vector<std::string>(begin, end);
  return vstrings;
};


inline int ParseFile(std::string f, int row, int col, std::string &result) {
  std::ifstream IF;
  std::vector<std::string> vstring;
  IF.open(f.c_str());
  if (isFileEmpty(IF)) {
    std::cerr << "System ERROR: cannot read file: " << f << std::endl;
    return 1;
  }
  std::string line;
  for (int i = 0; i < row; i++) getline(IF, line);
  vstring = SplitLine(line);
  result = vstring[col - 1];
  IF.close();
  return 0;
};
}
#endif
/*
int main(){
  string res;
  ParseFile("demo.txt",3,2,res);
  cout<<res<<endl;
   ParseFile("demo.txt",3,3,res);
  cout<<res<<endl;

}
*/
