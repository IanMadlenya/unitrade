//
//  StkHeader.h
//  InventoryManagment
//
//  Created by Bin Fan on 7/2/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef _unitrade_STK_Header_h
#define _unitrade_STK_Header_h

#include <iostream>
#include "ComHeader.h"
#include "library/bin.h"

namespace unitrade {

void call_from_thread_stk_quote(const std::string name);
void call_from_thread_stk_compute(const std::string name);
void call_from_thread_stk_execute(const std::string name);

class NormalTradeEngine;
class NormalExecuteEngine;

class Stock;
class Index;
class Logs;

typedef std::map<std::string, std::shared_ptr<Stock>> StockList;
typedef std::map<std::string, std::shared_ptr<Index>> IndexList;

class StockConfigPara {
 public:
  std::string ACCOUNT;
  std::string dbcn;
  std::string runtime_config;
  std::string passwd;
  std::string fund_account;
  std::string shanghai_account;
  std::string shenzhen_account;
  std::string stk_workspace;
  std::string stock_book;
  std::string index_list;
  std::string repo_list;
  std::string task_list;
  std::string stk_list;
  std::string stk_compute_strategy;
  std::string stk_execute_strategy;
  std::string shanghai_quote;
  std::string shenzhen_quote;
  bin::Timer MarketStart;
  bin::Timer MarketEnd;
  bin::Timer NoonBreakStart;
  bin::Timer NoonBreakEnd;
  std::string FILE_TO_REPORT_ORDER;
  std::string FILE_TO_ORDER_DETAIL;
  int parseParameter(std::ifstream& config);
  int setParameter() { return 0; }
};


class Stock : public security {
 public:
  std::string name;  // stock name
  std::string code;  // stock code
  std::string gics;  // gics
  int exchange;      // 0 for shanghai, 1 for shenzhen
  long total_shares, float_shares;
  int market_type;
  // 0 for Shanghai MainBoard, 1 for Shenzhen Mainboard,
  // 2 for Shenzen Zhongxiao, 3 for Shenzhen Chuangye
  // 4 for Shanghai B board, 5 for Hongkong markets
  int status;  // 0 for normal, 1 for stop-up, -1 for stop-down, 2 for halt
  bool is_halt;
  bool is_remove;
  int is_stop;  // 0 for normal, 1 for stop-up, -1 for stop-down
  bool is_st;
  bool is_split;
  bool is_divident;

  double last_price;
  double prev_close, open;
  double high, low, vol;
  double cost, PL, DeltaPL;
  double bid1, bid2, bid3, bid4, bid5;
  double ask1, ask2, ask3, ask4, ask5;
  double bid1_vol, bid2_vol, bid3_vol, bid4_vol, bid5_vol;
  double ask1_vol, ask2_vol, ask3_vol, ask4_vol, ask5_vol;
  int prev_pos, pos;
  int target_pos, check_pos, offset_pos;
  double total_shares_perct, float_shares_perct;
  double amt_perct;
  double target_total_shares_perct, target_float_shares_perct;
  double target_amt_perct;
  void update() {}
  Stock(std::string n) {
    code = n;
    is_halt = false;
    is_remove = false;
    is_stop = false;
    is_st = false;
    is_split = false;
    is_divident = false;
    is_stop = 0;
    if (n[0] == '6') {
      exchange = 0;
      market_type = 0;
    } else if (n[0] == '3') {
      exchange = 1;
      market_type = 3;
    } else if (n[0] == '0' && n[1] == '0' && n[2] == '2') {
      exchange = 1;
      market_type = 2;
    } else if (n[0] == '0' && n[1] == '0' && n[2] != '2') {
      exchange = 1;
      market_type = 1;
    }
    status = 0;
    prev_pos = pos = 0;
    check_pos = offset_pos = 0;
    cost = PL = DeltaPL = 0;
    target_pos = 0;
    last_price = -1;
    prev_close = -1;
    target_total_shares_perct = target_float_shares_perct = target_amt_perct =
        0;
    total_shares_perct = float_shares_perct = amt_perct = 0;
  }
};

class Index {
 public:
  std::string name;  // stock name
  std::string code;  // stock code
  double open;
  double prev_close;
  double vol;
  double high;
  double low;
  double last_price;
  double PL;
  Index(std::string n) {
    code = n;
    open = prev_close = vol = high = low = last_price = PL = 0;
  }
  void compile() {
    if (prev_close != 0) PL = (last_price - prev_close) / prev_close;
  }
};

class StockBook {
 public:
  int count_shanghai, count_shenzhenm, count_shenzhen, count_zhongxiao,
      count_chuangye;
  int count_stock, count_interest, count_repo, count_index;
  double repo_balance;
  double cash_balance, cash_enable, cash_withdraw;
  double check_cash, offset_cash;
  double prev_cash_balance;
  double market_value, asset;
  double prev_market_value, prev_asset;
  double pos_pect;
  double target_pos_pect;
  bin::Timer Shanghai_quotestamp, Shenzhen_quotestamp;
  bin::Timer* Wallclock;
  bool holding_match;
  StockList _StockBook;
  IndexList _IndexBook;
  StockList _RepoBook;
  StockList _InterestBook;
  StockList st;
  StockList halt;
  StockList shanghai;
  StockList shenzhen;
  StockList zhongxiao;
  StockList chuangye;
  StockBook() {
    count_chuangye = 0;
    count_shanghai = 0;
    count_shenzhen = 0;
    count_shenzhenm = 0;
    count_chuangye = 0;
    count_stock = 0;
    count_repo = 0;
    count_index = 0;
    count_interest = 0;
    cash_balance = cash_enable = cash_withdraw = 0;
    check_cash = offset_cash = 0;
    market_value = asset = 0;
    repo_balance = 0;
    prev_market_value = prev_asset = prev_cash_balance = 0;
    pos_pect = target_pos_pect = 0;
    holding_match = true;
  }
  int ReadLocal(class Para& para) {
    if (!bin::isFileExist(para.p_stockconfig->stock_book)) {
      WarningMsg("local stock book does not exist");
      return -1;
    } else if (bin::isFileEmpty(para.p_stockconfig->stock_book)) {
      WarningMsg("local stock book is empty " + para.p_stockconfig->stock_book);
      return -1;
    }

    std::ifstream ifs;
    ifs.open(para.p_stockconfig->stock_book);
    std::vector<std::string> vs;
    std::string line;
    bool header = true;
    StockList::iterator it;
    while (getline(ifs, line)) {
      if (bin::CheckComment(line)) continue;
      vs = bin::SplitLine(line);
      if (vs[0] == "Code" || vs[0] == "code") {
        header = false;
        continue;
      }
      if (header) continue;
      if (vs[1] != "0") {
        it = _StockBook.find(vs[0]);
        if (it != _StockBook.end()) it->second->pos = atoi(vs[1].c_str());
        _InterestBook[vs[0]] = it->second;
        count_interest++;
      }
    }
    ifs.close();
    return 0;
  }
  int compile() {
    StockList::iterator stockbook_it;
    market_value = 0;
    for (stockbook_it = _StockBook.begin(); stockbook_it != _StockBook.end();
         stockbook_it++) {
      market_value +=
          stockbook_it->second->pos * stockbook_it->second->last_price;
    }
    asset = cash_balance + market_value + repo_balance;
    if (asset != 0) pos_pect = market_value / asset;
    return 0;
  }
  int check(Logs* files);
  int PrintToFile(std::ofstream& ifs) {
    ifs.seekp(0);
    ifs << std::setw(8) << "Date" << std::setw(16) << "Clock" << std::setw(16)
        << "ShanghaiStamp" << std::setw(16) << "ShenzhenStamp" << std::endl;
    ifs << std::setw(8) << bin::printDate() << std::setw(16)
        << Wallclock->printShortTime() << std::setw(16)
        << Shanghai_quotestamp.printShortTime() << std::setw(16)
        << Shenzhen_quotestamp.printShortTime() << std::endl;

    IndexList::iterator it;
    ifs << std::setw(6) << "Index" << std::setw(14) << "Price" << std::setw(9)
        << "PL" << std::setw(10) << "PLP" << std::endl;
    for (it = _IndexBook.begin(); it != _IndexBook.end(); ++it) {
      ifs << std::setiosflags(std::ios::fixed);
      ifs << std::setprecision(2);
      ifs << std::setw(6) << it->second->code << std::setw(14)
          << it->second->last_price << std::setw(9)
          << it->second->last_price - it->second->prev_close << std::setw(9)
          << it->second->PL * 100 << "%" << std::setw(4) << std::endl;
    }

    ifs << std::setw(14) << "Asset" << std::setw(15) << "Holding"
        << std::setw(16) << "Cash" << std::endl;
    ifs << std::setw(14) << asset << std::setw(15) << market_value
        << std::setw(16) << cash_balance << std::endl;

    return 0;
  }
  int PrintToBook(std::ofstream& ifs) {
    ifs.seekp(0);
    ifs << std::setw(8) << "Date" << std::setw(16) << "Clock" << std::setw(16)
        << "ShanghaiStamp" << std::setw(16) << "ShenzhenStamp" << std::endl;
    ifs << std::setw(8) << bin::printDate() << std::setw(16)
        << Wallclock->printShortTime() << std::setw(16)
        << Shanghai_quotestamp.printShortTime() << std::setw(16)
        << Shenzhen_quotestamp.printShortTime() << std::endl;
    ifs << std::setw(6) << "Code" << std::setw(9) << "Pos" << std::setw(9)
        << "PPos" << std::setw(9) << "TPos" << std::setw(9) << "Price"
        << std::endl;
    StockList::iterator it;
    for (it = _StockBook.begin(); it != _StockBook.end(); ++it) {
      ifs << std::setw(6) << it->second->code << std::setw(9) << it->second->pos
          << std::setw(9) << it->second->prev_pos << std::setw(9)
          << it->second->target_pos << std::setw(9) << it->second->last_price
          << std::endl;
    }

    return 0;
  }
};

class StockOrder {
 public:
  std::string ID;
  std::string code;  // target stock code
  std::shared_ptr<Stock> p_stock;
  int pos;
  int target_pos;
  int num_1, num_2;
  int direction;  // 1 for buy and -1 for sell
  int offset;     // 1 for open and -1 for close
  int priority;
  bool is_split_opposite;
  int status;
  // -1 for cancelled, 0 for ready,
  // 1 for sent, 2 for ack,
  // 3 for partly, 4 for all finished
  // 5 for cancelling, 6 for partly traded and cancelled, 7 for no trading and
  // cancelled
  int type;
  // -2 for no care for time but cost
  // 0 TWAP, 1 for inmitate close price, -1 for do it fast but careful
  // 2 for quick, 3 for as quick as possible for whatever cost
  StockOrder(std::string c, std::shared_ptr<Stock> p) {
    code = c;
    p_stock = p;
    int curnt = p->pos;
    int target = p->target_pos;
    // p_stock = stock_ptr
    if (curnt * target < 0) {
      is_split_opposite = true;
      num_1 = abs(target);
      num_2 = abs(curnt);
      offset = -1;
      direction = (curnt > 0 ? -1 : 1);
    } else {
      is_split_opposite = false;
      num_1 = abs(target - curnt);
      num_2 = 0;
      offset = (abs(target) - abs(curnt) > 0 ? 1 : -1);
      direction = (target > curnt ? 1 : -1);
    }
    priority = 0;
    ID = "StkOrd-" + bin::RandomString(5);
  }
};

class StockOrderBook {
 public:
  typedef std::map<std::string, std::shared_ptr<Stock>> StockList;
  typedef std::multimap<int, std::shared_ptr<StockOrder>> OrderList;
  int cnt_AllOrder, cnt_FinishedOrder, cnt_ActiveOrder, cnt_WaitingOrder;
  OrderList AllOrder;
  OrderList FinishedOrder;
  OrderList ActiveOrder;
  OrderList WaitingOrder;
  StockBook* p_stockbook;
  bool need_execute {false};
  bool is_new_order;
  StockOrderBook(StockBook* p_sb) {
    p_stockbook = p_sb;
    cnt_AllOrder = cnt_FinishedOrder = cnt_ActiveOrder = cnt_WaitingOrder = 0;
    is_new_order = false;
  }
  void generateOrder() {
    std::shared_ptr<StockOrder> p_order;
    StockList::iterator it = p_stockbook->_StockBook.begin();
    for (; it != p_stockbook->_StockBook.end(); ++it) {
      if (it->second->target_pos != it->second->pos) {
        p_order = std::make_shared<StockOrder>(it->second->code, it->second);
        AllOrder.insert(std::make_pair(p_order->priority, p_order));
        cnt_AllOrder++;
      }
    }
    if (cnt_AllOrder != 0) is_new_order = true;
  }
  void CompileOrder() {}
  std::string printSummary() {
    return bin::int2string(cnt_AllOrder) + " orders, " +
           bin::int2string(cnt_FinishedOrder) + " finished, " +
           bin::int2string(cnt_ActiveOrder) + " active";
  }
};

class StockTradeEvent : public Event {
 public:
  StockTradeEvent(std::string n, StockBook* p,
                  shared_ptr<StkComputeEngine> p_com_eng,
                  shared_ptr<StkExecuteEngine> p_exe_eng)
      : Event(n) {
    p_stockbook = p;
    p_orderbook = std::make_shared<StockOrderBook>(p);
    auto p1 = make_shared<StockComputeEvent>("StockCompute", p,
                                                      p_orderbook, p_com_eng);
    auto p2 = make_shared<StockExecuteEvent>("StockExecute", p,
                                                      p_orderbook, p_exe_eng);
    p2->AssignParent(this);
    p1->AssignNext(p1);
    p1->thread_id = ThreadID::StkCompute;
    p2->thread_id = ThreadID::StkExecute;
   
    sub_events.push_back(p1);
    sub_events.push_back(p2); 
    ID = "ST_" + bin::RandomString(5);
  }
  virtual void start();
  virtual void execute();
  virtual void finalize();
  shared_ptr<StockOrderBook> p_orderbook;
  StockBook* p_stockbook;
};

class QueryStockEvent : public Event {
 public:
  QueryStockEvent(std::string n, StockBook* p,
                  shared_ptr<StkExecuteEngine> p_exe)
      : Event(n) {
    p_stockbook = p;
    ID = "QS_" + bin::RandomString(5);
    // need_execute = false;
    p_stkexecuteengine = p_exe;
  }
  StockBook* p_stockbook;
  shared_ptr<StkExecuteEngine> p_stkexecuteengine;
  virtual void execute();
  virtual void start();
  virtual void finalize();
};

int StockBookFactory(StockBook& sb, Para& para, bin::Timer& wallclock);
}
#endif
