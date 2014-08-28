//
//  ComHeader.h
//  StockTrade
//
//  Created by Bin Fan on 7/23/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//
//

#ifndef _Unitrade_UTIL_Header_h
#define _Unitrade_UTIL_Header_h

#include "StkHeader.h"
#include "FutHeader.h"
//#include "TraderSpi.h"
//#include "MdSpi.h"

namespace unitrade {

class Logs {
 public:
  static Logs* getInstance() {
    static Logs instance;  // Guaranteed to be destroyed.
    // Instantiated on first use.
    return &instance;
  }
  std::ifstream config;
  std::ifstream stk_config, fut_config;
  std::ifstream task_list;
  std::ifstream local_stock_book;
  std::ifstream local_futures_book;
  std::ifstream risk_list;

  std::ofstream control_log;
  std::ofstream warning_log;
  std::ofstream error_log;
  std::ofstream stock_monitor;
  std::ofstream futures_monitor;
  std::ofstream stock_book;
  std::ofstream futures_book;
  std::ofstream stock_report_order;
  std::ofstream stock_order_detail;
  std::ofstream futures_report_order;
  std::ofstream futures_order_detail;
  std::ofstream stk_report_order;
  std::ofstream stk_order_detail;
  std::ofstream task_monitor;
  std::ofstream risk_monitor;

  void OpenIN(Para& para) {
    config.open(para.config);
    stk_config.open(para.extern_stkconfig);
    if (!stk_config.is_open())
      ErrorMsg("cannot open stk config" + para.extern_stkconfig);
    fut_config.open(para.extern_futconfig);
    if (!fut_config.is_open())
      ErrorMsg("cannot open fut config" + para.extern_futconfig);

    task_list.open(para.task_list);
    if (!task_list.is_open())
      ErrorMsg("cannot open task list" + para.task_list);

    local_stock_book.open(para.p_stockconfig->stock_book);
    if (!local_stock_book.is_open())
      ErrorMsg("cannot open local stockbook" + para.p_stockconfig->stock_book);
    local_futures_book.open(para.p_futuresconfig->futures_book);
    if (!local_futures_book.is_open())
      ErrorMsg("cannot open local futures" +
               para.p_futuresconfig->futures_book);

    risk_list.open(para.risk_management_list);
    if (!risk_list.is_open())
      ErrorMsg("cannot open risk rules" + para.risk_management_list);
  }
  void OpenOUT(Para& para) {
    stock_monitor.open(para.stk_monitor);
    if (!stock_monitor.is_open())
      ErrorMsg("cannot open stock monitor" + para.stk_monitor);

    futures_monitor.open(para.fut_monitor);
    if (!futures_monitor.is_open())
      ErrorMsg("cannot open futures monitor" + para.fut_monitor);
    stock_book.open(para.p_stockconfig->stock_book);
    if (!stock_book.is_open())
      ErrorMsg("cannot open stock book" + para.p_stockconfig->stock_book);
    futures_book.open(para.p_futuresconfig->futures_book);
    if (!futures_book.is_open())
      ErrorMsg("cannot open futures book" + para.p_futuresconfig->futures_book);
    task_monitor.open(para.task_monitor);
    if (!task_monitor.is_open())
      ErrorMsg("cannot open task_monitor" + para.task_monitor);

    futures_report_order.open(para.p_futuresconfig->FILE_TO_REPORT_ORDER);
    if (!futures_report_order.is_open())
      ErrorMsg("cannot open fut report order" +
               para.p_futuresconfig->FILE_TO_REPORT_ORDER);
    futures_order_detail.open(para.p_futuresconfig->FILE_TO_ORDER_DETAIL);
    if (!futures_order_detail.is_open())
      ErrorMsg("cannot open fut detail order" +
               para.p_futuresconfig->FILE_TO_ORDER_DETAIL);
    stk_report_order.open(para.p_stockconfig->FILE_TO_REPORT_ORDER);
    if (!stk_report_order.is_open())
      ErrorMsg("cannot open stk report order" +
               para.p_stockconfig->FILE_TO_REPORT_ORDER);
    stk_order_detail.open(para.p_stockconfig->FILE_TO_ORDER_DETAIL);
    if (!stk_order_detail.is_open())
      ErrorMsg("cannot open stk order detail" +
               para.p_stockconfig->FILE_TO_ORDER_DETAIL);

    risk_monitor.open(para.risk_monitor);
    if (!risk_monitor.is_open())
      ErrorMsg("cannot open risk monitor detail" + para.risk_monitor);

    error_log.open(para.error_log);
    if (!error_log.is_open())
      ErrorMsg("cannot open stock monitor file" + para.error_log);
    warning_log.open(para.warning_log);
    if (!warning_log.is_open())
      ErrorMsg("cannot open futures monitor file" + para.warning_log);
  }

  void CloseIN() {
    config.close();
    stk_config.close();
    fut_config.close();
    task_list.close();
    local_stock_book.close();
    local_futures_book.close();
    risk_list.close();
  }
  void CloseOUT() {
    error_log.close();
    warning_log.close();
    stock_monitor.close();
    futures_monitor.close();
    stock_book.close();
    futures_book.close();
    task_monitor.close();
    futures_report_order.close();
    futures_order_detail.close();
    stk_report_order.close();
    stk_order_detail.close();
    risk_monitor.close();
  }

 private:
  Logs(){};
  Logs(Logs& S);
  void operator=(const Logs& S);
};

class Risk {
 public:
  int parse(Logs& log);
  int check(StockBook& sb, FuturesBook& fb);
  void PrintToFile(std::ofstream&);
  bool is_boundary_solid;
  bool futures_is_margin_red_line_overflow;
  bool futures_is_margin_yellow_line_overflow;
  bool futures_will_margin_red_line_overflow;
  bool futures_will_margin_yellow_line_overflow;
  bool stock_is_float_shares_red_line_overflow;
  bool stock_is_float_shares_yellow_line_overflow;
  bool stock_is_perct_red_line_overflow;
  bool stock_is_perct_yellow_line_overflow;
  bool stock_will_float_shares_red_line_overflow;
  bool stock_will_float_shares_yellow_line_overflow;
  bool stock_will_perct_red_line_overflow;
  bool stock_will_perct_yellow_line_overflow;
  bool total_is_stock_futures_difference_red_overflow;
  bool total_is_stock_futures_difference_yellow_overflow;
  bool total_will_stock_futures_difference_red_overflow;
  bool total_will_stock_futures_difference_yellow_overflow;

  double futures_margin_red_line;
  double futures_margin_yellow;
  double stock_float_shares_red;
  double stock_float_shares_yellow;
  double stock_perct_red;
  double stock_perct_yellow;
  double total_stock_futures_difference_red;
  double total_stock_futures_difference_yellow;

  std::vector<std::shared_ptr<Futures> > yellow_stock_list;
  std::vector<std::shared_ptr<Futures> > red_stock_list;
};

typedef struct Calendar {
 public:
  bool isTradingDay{false};
  int horizontal{1500};
  std::vector<int> trading_days;
  std::vector<int> calendar_days;
  int Load(std::string cal);
} Calendar;

}
#endif
