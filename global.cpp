//
//  global.cpp
//  InventoryManagment
//
//  Created by Bin Fan on 7/6/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#include "global.h"
#include "StkHeader.h"
#include "FutHeader.h"
#include <map>
using namespace std;


namespace unitrade
{

std::map<std::string, int> thread_status = {{"ALL",0}, {"fut_quote",0},{"fut_compute",0}
    ,{"fut_execute",0},{"server",0}, {"monitor",0}, {"stk_quote",0}, {"stk_compute",0}, {"stk_execute",0}};
StockBook stock_book;
FuturesBook futures_book;

FuturesOrderBook futures_order;

Condition condition;

Para  para;
FuturesConfigPara fut_config;
StockConfigPara stk_config;

bin::Timer wall_clock;
bin::Timer quote_stamp;
bin::Date  wall_date;
mutex cv;
std::mutex print_mutex;


//CTPMd MD;
//CTPTrader TD;

int iRequestID=1;
int iTradeRequestID=1000;

bool is_compute_stk_ready(false);
bool is_execute_stk_ready(false);
bool is_compute_stk_finish(false);
bool is_execute_stk_finish(false);
bool is_compute_fut_ready(false);
bool is_execute_fut_ready(false);
bool is_compute_fut_finish(false);
bool is_execute_fut_finish(false);
bool is_monitor_ready(false);
std::mutex lk_fut_compute;
std::mutex lk_stk_compute;
std::mutex lk_fut_execute;
std::mutex lk_stk_execute;
std::mutex lk_monitor;
std::condition_variable cv_fut_compute;
std::condition_variable cv_stk_execute;
std::condition_variable cv_fut_execute;
std::condition_variable cv_stk_compute;
std::condition_variable cv_monitor;

std::map<ThreadID, std::mutex*> mutex_map =
   { {ThreadID::FutCompute, &lk_fut_compute}, {ThreadID::FutExecute, &lk_fut_execute}, 
     {ThreadID::StkCompute, &lk_stk_compute}, {ThreadID::StkExecute, &lk_stk_execute}
   };

std::map<ThreadID, std::condition_variable*> cv_map = 
   { {ThreadID::FutCompute, &cv_fut_compute}, {ThreadID::FutExecute, &cv_fut_execute}, 
     {ThreadID::StkCompute, &cv_stk_compute}, {ThreadID::StkExecute, &cv_stk_execute}
  };


}
