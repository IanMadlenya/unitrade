//
//  element.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/23/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include "ComHeader.h"
#include "StkHeader.h"
#include "StkCompute.h"
#include "global.h"
#include <fstream>

using namespace std;

namespace unitrade
{

void ManualStkComputeEngine::execute(
    std::shared_ptr<StockOrderBook> p_orderbook) {
  Msg("stk_compute", "manual strategy initiated");
  ParseTarget();
  Msg("stk_compute", "generating orders");
  p_orderbook->generateOrder();
  need_execute = p_orderbook->is_new_order;
}

void ManualStkComputeEngine::ParseManual() {
  ifstream ifs;
  ifs.open(bin::FolderAppend(para.p_stockconfig->stk_compute_strategy,
                        name + ".strategy"));
  std::string ss;
  bin::ParseConfig<string>("source", ss, ifs);
  source.open(ss);
  bin::ParseConfig<float>("extent", extent, ifs);
}

void ManualStkComputeEngine::ParseTarget() {
  Msg("stk_compute", "parsing external source");
  source.seekg(0);
  bool header = true;
  vector<string> vs;
  string line;
  while (getline(source, line)) {
    if (bin::CheckComment(line)) continue;
    if (header) {
      header = false;
      continue;
    }
    vs = bin::SplitLine(line);
    StockList::iterator it;
    it = stock_book._StockBook.find(vs[0]);
    if (it != stock_book._StockBook.end())
      it->second->target_pos = (int)(atoi(vs[1].c_str()) * extent);
  }
}

void NormalStkComputeEngine::execute(
    std::shared_ptr<StockOrderBook> p_orderbook) {
  Msg("stk_compute", "normal strategy initiated, computation starts...");

  std::clock_t start = std::clock();
  double duration;

  ///
  ///    computation algorithm
  ///

  duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
  Msg("stk_compute", "computation finished, takes " +
                         bin::double2string(duration) + " seconds, " +
                         bin::double2string((std::clock() - start)) + " ticks");
  cout << (double)CLOCKS_PER_SEC << endl;
  Msg("stk_compute", "generating orders");
  p_orderbook->generateOrder();
  need_execute = p_orderbook->is_new_order;
}

}
