#ifndef _bin_parseconfig_
#define _bin_parseconfig_ 1


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <type_traits>
using namespace std;

namespace bin {

struct TestEOL {
  bool operator()(char c) {
    last = c;
    return last == '\n';
  }
  char last;
};

inline size_t CountLine(std::string name) {
  std::fstream fs;
  fs.open(name, std::fstream::in | std::fstream::binary);
  TestEOL te;
  std::size_t count = std::count_if(std::istreambuf_iterator<char>(fs),
                                    std::istreambuf_iterator<char>(), te);

  return count;
};

inline size_t CountLine(std::ifstream& ifs) {
  TestEOL te;
  std::size_t count = std::count_if(std::istreambuf_iterator<char>(ifs),
                                    std::istreambuf_iterator<char>(), te);

  ifs.seekg(0, ifs.beg);
  return count;
};

inline std::string FolderAppend(std::string folder, std::string file) {
  if (folder[folder.size() - 1] != '/') folder += "/";
  if (folder[folder.size() - 1] == '/' && file[0] == '/')
    return folder.substr(0, folder.size() - 1) + file;
  else
    return folder + file;
};

template<typename T>
inline int ParseConfig(string name, T& value, ifstream& FILE) {
  size_t pFound, pFound2, pFound3, pFound4;
  std::string line, dname, dvalue;
  int FLAG = 0;  // use to mark if the name is unique
  std::string content;
  while (std::getline(FILE, line)) {
    if (line == "") continue;
    pFound = line.find_first_not_of(" \t");
    if (pFound != std::string::npos && line[pFound] != '[' && line[pFound] != '#') {
      // for the lines that are not only comment or tags
      // find the first string before : or = as the data name field
      pFound2 = line.find_first_of(" :=", pFound);
      dname = line.substr(pFound, pFound2 - pFound);
      if (dname == name) {
        pFound3 = line.find_first_not_of(" =:", pFound2);
        pFound4 = line.find_first_of(" ;#\t!", pFound3);
        content = line.substr(pFound3, pFound4 - pFound3);
        value = string_to<T>(content);
      
        FLAG++;
      }
    }
  }

  // finish processing file
  if (!FILE.eof()) {
    cerr << "Parse ERROR: Process terminated before reaching file end" << endl;
    FILE.clear();
    FILE.seekg(0);  // rewind to the beginning
  
    return -1;
  } else if (FLAG >= 2) {
    cerr << "Parse Warning: ID is not unique, return last value" << endl;
    FILE.clear();
    FILE.seekg(0);  // rewind to the beginning
    return 0;
  } else if (FLAG == 0) {
    cerr << "Parse ERROR: cannot find corresponding value: " << name << endl;
   
    FILE.clear();
    FILE.seekg(0);  // rewind to the beginning
    return -1;
  }
  FILE.clear();
  FILE.seekg(0);  // rewind to the beginning
  return 0;
}


}

#endif
/*
int main()
{
        ifstream IF;
        IF.open("demo.txt");
        string key;
        vector<string> value;
        key="pos";
        ParseConfigList(key, value, IF);
        for (int i=0; i<value.size();i++)
        cout<<value[i]<<endl;
}
*/
