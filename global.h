//
//  global.h
//  InventoryManagment
//
//  Created by Bin Fan on 7/6/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef __InventoryManagment__global__
#define __InventoryManagment__global__

#include <iostream>
#include <map>
#include "library/bin.h"
#include "StkHeader.h"
#include "FutHeader.h"
#include "UtilHeader.h"
#include "quote.h"

typedef std::map<std::string, class Stock* > StockBookType;


//extern CTPMd MD;
//extern CTPTrader TD;

namespace unitrade
{

extern StockBook stock_book;
extern FuturesBook futures_book;
extern FuturesOrderBook futures_order;


extern Condition condition;

extern std::map<std::string, class Order* > internal_order_list;
extern std::map<std::string, int> thread_status;
extern bin::Timer wall_clock;
extern bin::Date wall_date;
extern bin::Timer quote_stamp;
extern Para  para;
extern FuturesConfigPara fut_config;
extern StockConfigPara stk_config;

extern std::mutex print_mutex;
extern std::mutex stockbook_mutex;
extern std::mutex stkexit_mutex;
extern std::mutex futexit_mutex;
extern bool stk_exit ;
extern bool fut_exit ;
extern Logs* FileLogs;
extern EventsByTime tasks;
extern std::mutex cv;

extern bool is_compute_stk_ready;
extern bool is_execute_stk_ready;
extern bool is_compute_fut_ready;
extern bool is_execute_fut_ready;
extern bool is_monitor_ready;

extern std::mutex lk_fut_compute;
extern std::mutex lk_stk_compute;
extern std::mutex lk_fut_execute;
extern std::mutex lk_stk_execute;
extern std::mutex lk_monitor;

extern std::condition_variable cv_fut_compute;
extern std::condition_variable cv_stk_execute;
extern std::condition_variable cv_fut_execute;
extern std::condition_variable cv_stk_compute;
extern std::condition_variable cv_monitor;

extern bool is_compute_stk_finish;
extern bool is_execute_stk_finish;
extern bool is_compute_fut_finish;
extern bool is_execute_fut_finish;

extern std::map<ThreadID, std::mutex*> mutex_map;
extern std::map<ThreadID, std::condition_variable*> cv_map; 
}
#endif /* defined(__InventoryManagment__global__) */
