//
//  element.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/23/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include "global.h"
#include "ComHeader.h"
#include "StkHeader.h"
#include "StkCompute.h"
#include "StkExecute.h"
#include <fstream>

using namespace std;

namespace unitrade {

int StockBook::check(Logs* files) {
  StockList::iterator it;
  files->warning_log << "-----------" << std::endl;
  files->warning_log << "[" << bin::printTime()
                     << "] StockAccountCheck:" << std::endl;
  if (abs(cash_balance - check_cash) < 10)
    files->warning_log << "Cash: MATCH local=" << cash_balance
                       << " vs. remote=" << check_cash << std::endl;
  for (it = _StockBook.begin(); it != _StockBook.end(); it++) {
    it->second->offset_pos = (it->second->check_pos - it->second->pos);
    if (it->second->offset_pos != 0)
      files->warning_log << it->second->code
                         << ": Mismatch local=" << it->second->pos
                         << " vs. remote=" << it->second->check_pos
                         << std::endl;
  }
  return 0;
}

int StockConfigPara::parseParameter(ifstream& file_config) {

  string spara;
  if (bin::ParseConfig("FUND_ACCOUNT", fund_account, file_config) == -1)
    return -1;
  Msg("core", "set fund_account=" + fund_account);
  bin::ParseConfig("SHANGHAI_ACCOUNT", shanghai_account, file_config);
  Msg("core", "set shanghai_account=" + shanghai_account);
  bin::ParseConfig("SHENZHEN_ACCOUNT", shenzhen_account, file_config);
  Msg("core", "set shenzhen_account=" + shenzhen_account);
  if (bin::ParseConfig("PASSWD", passwd, file_config) == -1) return -1;
  Msg("core", "set STOCK PASSWD");

  bin::ParseConfig("shhq", shanghai_quote, file_config);
  Msg("core", "set shhq=" + shanghai_quote);

  bin::ParseConfig("szhq", shenzhen_quote, file_config);
  Msg("core", "set szhq=" + shenzhen_quote);

  if (bin::ParseConfig("STOCK_LIST", stk_list, file_config) == -1) return -1;
  stk_list = bin::FolderAppend(stk_workspace, stk_list);
  if (!bin::isFileExist(stk_list)) {
    FatalErrorMsg("stock list file does not exist!");
    return -1;
  }
  Msg("core", "set stk list=" + stk_list);

  bin::ParseConfig("INDEX_LIST", index_list, file_config);
  index_list = bin::FolderAppend(stk_workspace, index_list);
  Msg("core", "set index list=" + index_list);

  bin::ParseConfig("REPO_LIST", repo_list, file_config);
  repo_list = bin::FolderAppend(stk_workspace, repo_list);
  Msg("core", "set repo list=" + repo_list);

  bin::ParseConfig("STOCK_BOOK", stock_book, file_config);
  stock_book = bin::FolderAppend(stk_workspace, stock_book);
  Msg("core", "set stock_book=" + stock_book);

  bin::ParseConfig("STOCK_COMPUTE_STRATEGY", stk_compute_strategy, file_config);
  stk_compute_strategy = bin::FolderAppend(stk_workspace, stk_compute_strategy);
  Msg("core", "set stk compute dir=" + stk_compute_strategy);

  bin::ParseConfig("STOCK_EXECUTE_STRATEGY", stk_execute_strategy, file_config);
  stk_execute_strategy = bin::FolderAppend(stk_workspace, stk_execute_strategy);
  Msg("core", "set stk execute dir=" + stk_execute_strategy);

  bin::ParseConfig("CONFIG_RUNTIME", runtime_config, file_config);
  runtime_config = bin::FolderAppend(stk_workspace, runtime_config);
  Msg("core", "set stk runtime=" + runtime_config);

  bin::ParseConfig("MARKET_START_TIME", spara, file_config);
  MarketStart.set(spara);
  bin::ParseConfig("MARKET_END_TIME", spara, file_config);
  MarketEnd.set(spara);
  Msg("core", "Stk Market Time " + MarketStart.printShortTime() + "->" +
                  MarketEnd.printShortTime());
  bin::ParseConfig("NOON_BREAK_START_TIME", spara, file_config);
  NoonBreakStart.set(spara);
  bin::ParseConfig("NOON_BREAK_END_TIME", spara, file_config);
  NoonBreakEnd.set(spara);
  Msg("core", "Stk Noon Break " + NoonBreakStart.printShortTime() + "->" +
                  NoonBreakEnd.printShortTime());

  bin::ParseConfig("STOCK_REPORT_ORDER", FILE_TO_REPORT_ORDER, file_config);
  FILE_TO_REPORT_ORDER = bin::FolderAppend(stk_workspace, FILE_TO_REPORT_ORDER);
  Msg("core", "set futures report order=" + FILE_TO_REPORT_ORDER);

  bin::ParseConfig("STOCK_ORDER_DETAIL", FILE_TO_ORDER_DETAIL, file_config);
  FILE_TO_ORDER_DETAIL = bin::FolderAppend(stk_workspace, FILE_TO_ORDER_DETAIL);
  Msg("core", "set futures order detail=" + FILE_TO_ORDER_DETAIL);
  return 0;
}

int StockBookFactory(StockBook& sb, Para& para, bin::Timer& clock) {
  sb.Wallclock = &clock;
  ifstream ifs(para.p_stockconfig->stk_list);
  if (!ifs.is_open()) {
    FatalErrorMsg(" cannot open stk list file -" +
                  para.p_stockconfig->stk_list);
    return 2;
  }

  string line;
  getline(ifs, line);
  vector<string> vstring;
  while (getline(ifs, line)) {
    vstring = bin::SplitLine(line);
    sb._StockBook[vstring[1]] = make_shared<Stock>(vstring[1]);
    // sb._StockBook[vstring[1]]->name = vstring[8];
    if (vstring[1][0] == '6') {
      sb.shanghai[vstring[1]] = sb._StockBook[vstring[1]];
      sb.count_shanghai++;
    } else if (vstring[1][0] == '0' && vstring[1][2] != '2') {
      sb.shenzhen[vstring[1]] = sb._StockBook[vstring[1]];
      sb.count_shenzhenm++;
      sb.count_shenzhen++;
    } else if (vstring[1][0] == '3') {
      sb.chuangye[vstring[1]] = sb._StockBook[vstring[1]];
      sb.count_chuangye++;
      sb.count_shenzhen++;
    } else if (vstring[1][0] == '0' && vstring[1][2] == '2') {
      sb.zhongxiao[vstring[1]] = sb._StockBook[vstring[1]];
      sb.count_zhongxiao++;
      sb.count_shenzhen++;
    }
    sb.count_stock++;
  }
  sb._StockBook["888880"] =
      make_shared<Stock>("888880");  // the special entry for repo
  ifstream ifr(para.p_stockconfig->repo_list);
  if (!ifr.is_open()) {
    FatalErrorMsg(" cannot open repo list file -" +
                  para.p_stockconfig->repo_list);
    return 2;
  }

  getline(ifr, line);
  while (getline(ifr, line)) {
    vstring = bin::SplitLine(line);
    sb._RepoBook[vstring[0]] = make_shared<Stock>(vstring[0]);
    sb.count_repo++;
  }

  ifstream ift(para.p_stockconfig->index_list);
  if (!ift.is_open()) {
    FatalErrorMsg(" cannot open index list file -" +
                  para.p_stockconfig->index_list);
    return 2;
  }

  getline(ift, line);
  while (getline(ift, line)) {
    vstring = bin::SplitLine(line);
    sb._IndexBook[vstring[0]] = make_shared<Index>(vstring[0]);
    sb._IndexBook[vstring[0]]->name = vstring[1];
    sb.count_index++;
  }
  return 0;
}


void StockTradeEvent::start() {
  // p_stkexecuteengine->execute(p_orderbook);
  beg = wall_clock;
  execute();
};


void StockTradeEvent::execute() {
  // p_stkexecuteengine->execute(p_orderbook);
  if (sub_events.size()!=0)
    (*sub_events.begin())->start();
  else 
    finalize();
};


void StockTradeEvent::finalize(){
  end = wall_clock;
  des_status = "success";
  if (next_event)
    next_event->start();
  if (next_parent)
    next_parent->finalize();
};



void StockComputeEvent::start() {
  beg = wall_clock;
  des_status = "active";
  Msg("core", "StockComputeEvent starts");
  // notify computation to start
  std::unique_lock<std::mutex> lk(lk_stk_compute);
  is_compute_stk_ready = true;
  tasks.on_stk_compute_event = this;
  cv_stk_compute.notify_one();
};


void StockComputeEvent::execute() {
  p_stkcomputeengine->execute(p_orderbook);
  finalize();
};


void StockComputeEvent::finalize() {
  p_orderbook->need_execute = p_stkcomputeengine->need_execute; // see if further
  // execution is needed
  Msg("stk_compute", p_orderbook->printSummary());
  if (need_execute) {
    Msg("stk_compute", "orders generated, start execution");
  } else {
    Msg("stk_compute", "no further execution required");
    des_status = "success";
    end = wall_clock;
  }
  if (next_event)
    next_event->start();
  if (next_parent)
    next_parent->finalize();
};


/* stock execute event  */
void StockExecuteEvent::start() {
  beg = wall_clock;
  des_status = "active";
  Msg("core", "StockExecuteEvent starts");
  // notify computation to start
  std::unique_lock<std::mutex> lk(lk_stk_execute);
  is_execute_stk_ready = true;
  tasks.on_stk_execute_event = this;
  cv_stk_execute.notify_one();
};


void StockExecuteEvent::execute() {
  if (p_orderbook->need_execute)
    p_stkexecuteengine->execute(p_orderbook);
  else
    cerr<<"--->>> StockExecuteEvent: no executation"<<endl;
  finalize();
};


void StockExecuteEvent::finalize() {
  end = wall_clock;
  des_status = "successful"; 
  // execution is needed
  if (next_event)
    next_event->start();
  if (next_parent)
    next_parent->finalize();
};



/* query stock event  */
void QueryStockEvent::start() {
  beg = wall_clock;
  des_status = "active";
  Msg("core", "QueryStock starts");
  std::unique_lock<std::mutex> lk(*(mutex_map[thread_id]));
  is_execute_stk_ready = true;
  tasks.on_stk_execute_event = this;
  cv_map[thread_id]->notify_one();
};


void QueryStockEvent::execute() {
  p_stkexecuteengine->execute();
  finalize();
};

void QueryStockEvent::finalize() {
  end = wall_clock;
  try_times++;
  des_status = "success";
  if (next_event)
    next_event->start();
  if (next_parent)
    next_parent->finalize();
};



}
