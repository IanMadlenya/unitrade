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
#include "FutHeader.h"
#include "FutCompute.h"
#include "FutExecute.h"
#include <fstream>

using namespace std;

namespace unitrade {

void Condition::Update(bin::Timer& clock, Para& para) {
  if (clock.isBetween(para.p_futuresconfig->EstimateSettleStart,
                      para.p_futuresconfig->EstimateSettleEnd))
    is_estimate_settlement = true;
}

Event::Event(const std::string n) {
  name = n;
  status = 1;
  des_status = "OnMark";
  max_times = try_times = 0;
  // need_execute = false;
}

std::shared_ptr<Event> EventFactory(const vector<std::string> vs) {
  shared_ptr<Event> newevent;
  if (vs[0] == "stock_trade") {
    newevent = make_shared<StockTradeEvent>(vs[0], &stock_book,
                                            StkComputeEngineFactory(vs[2]),
                                            StkExecuteEngineFactory(vs[3]));
  } else if (vs[0] == "futures_trade") {
    newevent = make_shared<FuturesTradeEvent>(
        vs[0], FutComputeEngineFactory(vs[2]), FutExecuteEngineFactory(vs[3]));
    newevent->thread_id = ThreadID::FutExecute;
  } else if (vs[0] == "query_stock") {
    newevent = make_shared<QueryStockEvent>(
        vs[0], &stock_book, StkExecuteEngineFactory("QueryStock"));
    newevent->thread_id = ThreadID::StkExecute;
  } else if (vs[0] == "query_futures") {
    newevent = make_shared<QueryFuturesEvent>(
        vs[0], FutExecuteEngineFactory("QueryFutures"));
   newevent->thread_id = ThreadID::FutExecute;  
  } else if (vs[0] == "futures_settlement") {
    newevent = make_shared<QueryFuturesSettlementEvent>(
        vs[0], FutExecuteEngineFactory("QueryFuturesSettlement"));
    newevent->thread_id = ThreadID::FutExecute;
  } else {
    WarningMsg("cannot parse event" + vs[0]);
  }

  if (vs[1] == "start") {
    newevent->type = "OnStart";
    newevent->time_type = bin::TaskTimeType::OnStart;
  } else if (vs[1] == "stop") {
    newevent->type = "OnStop";
    newevent->time_type = bin::TaskTimeType::OnStop;
  } else {
    newevent->mark.set(vs[1]);
    newevent->type = "OnTime";
    newevent->time_type = bin::TaskTimeType::OnTime;
  }
  newevent->Inherit();
  return newevent;
}

std::shared_ptr<StkExecuteEngine> StkExecuteEngineFactory(std::string vs) {
  std::shared_ptr<StkExecuteEngine> newengine;
  if (vs == "normal") {
    newengine = make_shared<StkNormalExecuteEngine>();
  } else if (vs == "QueryStock") {
    newengine = make_shared<QueryStockExecuteEngine>();
  } else {
    ErrorMsg("cannot initialize execute engine for " + vs);
  }
  return newengine;
}

std::shared_ptr<StkComputeEngine> StkComputeEngineFactory(std::string vs) {
  std::shared_ptr<StkComputeEngine> newengine;
  if (vs == "normal") {
    newengine = make_shared<NormalStkComputeEngine>();
  } else if (vs == "manual") {
    newengine = make_shared<ManualStkComputeEngine>();
  } else {
    ErrorMsg("cannot initialize execute engine for " + vs);
  }
  return newengine;
}

std::shared_ptr<FutExecuteEngine> FutExecuteEngineFactory(std::string vs) {
  std::shared_ptr<FutExecuteEngine> newengine;
  if (vs == "QueryFutures") {
    newengine = make_shared<QueryFuturesExecuteEngine>();
  } else if (vs == "QueryFuturesSettlement") {
    newengine = make_shared<QueryFuturesSettlementExecuteEngine>();
  } else if (vs == "normal") {
    newengine = make_shared<NormalFuturesExecuteEngine>();
  } else {
    ErrorMsg("cannot initialize execute engine for " + vs);
  }
  return newengine;
}

std::shared_ptr<FutComputeEngine> FutComputeEngineFactory(std::string vs) {
  std::shared_ptr<FutComputeEngine> newengine;
  if (vs == "TWAP") {
    newengine = make_shared<TWAPFuturesComputeEngine>();
  } else if (vs == "FastRoll") {
    newengine = make_shared<FastRollFuturesComputeEngine>();
  }
  return newengine;
}

void EventsByTime::ParseEvent(std::ifstream& ifs) {
  std::string line;
  std::vector<std::string> vs;
  bool header = true;
  EventList::iterator it;
  shared_ptr<Event> p_e;
  while (getline(ifs, line)) {
    if (bin::CheckComment(line)) continue;
    if (header) {
      header = false;
      continue;
    }
    vs = bin::SplitLine(line);
    if (vs[1] == "start")
      OnStart.push_back(EventFactory(vs));
    else if (vs[1] == "stop")
      OnStop.push_back(EventFactory(vs));
    else {
      p_e = EventFactory(vs);
      it = events.insert(std::pair<long, shared_ptr<Event>>(
          bin::ParseTimeToSecond(vs[1]), p_e));
      if (wall_clock.earlierThan(vs[1]))
        event_schedule.insert(std::pair<long, shared_ptr<Event>>(
            bin::ParseTimeToSecond(vs[1]), p_e));
      else
      {
        it->second->des_status = "LATE_PASSED";
        it->second->Spread();
      }
    }
  }
}

void EventsByTime::Try() {
  EventList::iterator it = event_schedule.begin();
  while (it != event_schedule.end() &&
         it->second->mark.earlierThan(wall_clock)) {
    on_event = it->second;
    Msg("core", it->second->name + " will execute, ID=" + it->second->ID +
                    ", [" + bin::printTime() + "]");
    it->second->start();
    event_schedule.erase(it++);
  }
}

int Para::parse() {
  std::ifstream file_config(config);
  if (!file_config.is_open()) {
    FatalErrorMsg(" cannot open config file -- " + config);
    return 1;
  }

  struct tm* timeval;
  time_t tt;
  tt = time(NULL);
  timeval = localtime(&tt);
  day_of_week = timeval->tm_wday;
  day_of_month = timeval->tm_mday;
  day = timeval->tm_yday;
  month = timeval->tm_mon + 1;
  year = timeval->tm_year + 1900;

  std::string temp;

  bin::ParseConfig<string>("ACCOUNT", ACCOUNT, file_config);
  Msg("core", "set account=" + ACCOUNT);

  bin::ParseConfig<std::string>("DBCN", dbcn, file_config);
  Msg("core", "set dbcn=" + dbcn);

  bin::ParseConfig<std::string>("STOCK_WORKSPACE", stk_workspace, file_config);
  Msg("core", "set stk workspace=" + stk_workspace);
  bin::ParseConfig<std::string>("FUTURES_WORKSPACE", fut_workspace,
                                file_config);
  Msg("core", "set fut workspace=" + fut_workspace);

  bin::ParseConfig<std::string>("WORKSPACE", workspace, file_config);
  Msg("core", "set workspace=" + workspace);

  bin::ParseConfig<std::string>("RISK_MANAGEMENT", risk_management_list,
                                file_config);
  risk_management_list = bin::FolderAppend(workspace, risk_management_list);
  Msg("core", "set risk=" + risk_management_list);

  bin::ParseConfig<std::string>("RISK_MONITOR", risk_monitor, file_config);
  risk_monitor = bin::FolderAppend(workspace, risk_monitor);
  Msg("core", "set risk monitor=" + risk_monitor);

  bin::ParseConfig<std::string>("LOCK_FILE", locker, file_config);
  locker = bin::FolderAppend(workspace, locker);
  Msg("core", "set locker=" + locker);

  bin::ParseConfig<std::string>("ERROR_LOG", error_log, file_config);
  error_log = bin::FolderAppend(workspace, error_log);
  Msg("core", "set error_log=" + error_log);

  bin::ParseConfig<std::string>("WARNING_LOG", warning_log, file_config);
  warning_log = bin::FolderAppend(workspace, warning_log);
  Msg("core", "set warning_log=" + warning_log);

  bin::ParseConfig<std::string>("STOCK_MONITOR", stk_monitor, file_config);
  stk_monitor = bin::FolderAppend(workspace, stk_monitor);
  Msg("core", "set stk monitor=" + stk_monitor);

  bin::ParseConfig<std::string>("FUTURES_MONITOR", fut_monitor, file_config);
  fut_monitor = bin::FolderAppend(workspace, fut_monitor);
  Msg("core", "set fut monitor=" + fut_monitor);

  bin::ParseConfig<std::string>("FILE_TO_COMMAND_SOCKET", command_socket,
                                file_config);
  command_socket =
      bin::FolderAppend(fut_workspace, ACCOUNT + "_" + command_socket);

  bin::ParseConfig<std::string>("TASK_MONITOR", task_monitor, file_config);
  task_monitor = bin::FolderAppend(workspace, task_monitor);
  Msg("core", "set task monitor=" + task_monitor);

  bin::ParseConfig<std::string>("TASK_LIST", task_list, file_config);
  task_list = bin::FolderAppend(workspace, task_list);
  Msg("core", "set task_list=" + task_list);

  bin::ParseConfig<std::string>("EXTERN_STKCONFIG", extern_stkconfig,
                                file_config);
  extern_stkconfig = bin::FolderAppend(stk_workspace, extern_stkconfig);
  Msg("core", "set extern stkconfig=" + extern_stkconfig);
  if (!bin::isFileExist(extern_stkconfig)) {
    FatalErrorMsg("cannot find stk config=" + extern_stkconfig);
    return -1;
  }

  bin::ParseConfig<std::string>("EXTERN_FUTCONFIG", extern_futconfig,
                                file_config);
  extern_futconfig = bin::FolderAppend(fut_workspace, extern_futconfig);
  Msg("core", "set extern futconfig=" + extern_futconfig);
  if (!bin::isFileExist(extern_futconfig)) {
    FatalErrorMsg("cannot find fut config=" + extern_futconfig);
    return -1;
  }

  bin::ParseConfig<std::string>("EXIT_TIME", temp, file_config);
  exit_time.set(temp);
  Msg("core", "set exit time=" + exit_time.printShortTime());

  p_stockconfig = &stk_config;
  p_futuresconfig = &fut_config;
  p_futuresconfig->fut_workspace = fut_workspace;
  p_stockconfig->stk_workspace = stk_workspace;
  p_stockconfig->ACCOUNT = ACCOUNT;
  p_futuresconfig->ACCOUNT = ACCOUNT;
  p_stockconfig->dbcn = dbcn;
  p_futuresconfig->dbcn = dbcn;

  std::ifstream ifs1, ifs2;
  ifs1.open(extern_stkconfig);
  ifs2.open(extern_futconfig);

  if (p_stockconfig->parseParameter(ifs1) == -1) return -1;
  if (p_futuresconfig->parseParameter(ifs2) == -1) return -1;
  ifs1.close();
  ifs2.close();

  return 0;
}
}
