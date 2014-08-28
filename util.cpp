//
//  util.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/22/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include "StkHeader.h"
#include <iostream>
#include <string>
#include "global.h"
#include "library/bin.h"

namespace unitrade {

int FatalErrorMsg(const std::string msg) {
  print_mutex.lock();
  std::cerr << "!-->>> FATAL ERROR: " << msg << std::endl;
  print_mutex.unlock();
  return 1;
}

int ErrorMsg(const std::string msg) {
  print_mutex.lock();
  std::cerr << "!-->>> ERROR: " << msg << std::endl;
  print_mutex.unlock();
  return 1;
}

int WarningMsg(const std::string msg) {
  print_mutex.lock();
  std::cerr << "!-->>> Warning: " << msg << std::endl;
  print_mutex.unlock();
  return 1;
}

int Msg(const std::string sub, const std::string msg) {
  print_mutex.lock();
  std::cerr << "--->>> " << sub << ": " << msg << std::endl;
  print_mutex.unlock();
  return 0;
}

bool isLeapYear(int year) {
  return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0);
}

int Calendar::Load(std::string cal) {
  Msg("core", "load calendar info");
  std::ifstream IF;
  std::string line;
  bin::Date mydate;
  int today = atoi(bin::printDate(mydate).c_str());
  if (!bin::isFileExist(cal)) {
    FatalErrorMsg("cal file does not exist!--> " + cal);
    // stk_exit = true;
    // fut_exit = true;
    return -1;
  }
  IF.open(cal.c_str());
  trading_days.reserve(1752);
  while (IF.good()) {
    getline(IF, line);
    trading_days.push_back(atoi(line.c_str()));
  }

  std::vector<int>::iterator it, mark;
  if ((it = std::find(trading_days.begin(), trading_days.end(), today)) !=
      trading_days.end()) {
    isTradingDay = true;
    mark = it;
  } else {
    isTradingDay = false;
    mark = std::upper_bound(trading_days.begin(), trading_days.end(), today);
  }
  size_t before_today = (std::distance(trading_days.begin(), mark) > 1500
                             ? 1500
                             : std::distance(trading_days.begin(), mark));
  size_t after_today = (std::distance(mark, trading_days.end()) > 250
                            ? 250
                            : std::distance(mark, trading_days.end()));
  std::vector<int>(mark - before_today+1, mark + after_today-1).swap(trading_days);
  IF.close();
  return 0;
}

int CheckSettletDay(Calendar &cal){


}

}
