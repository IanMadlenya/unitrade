//
//  ComHeader.h
//  StockTrade
//
//  Created by Bin Fan on 7/23/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//
//

#ifndef _unitrade_ComHeader_h
#define _unitrade_ComHeader_h

#include <cmath>
#include <string>
#include <map>
#include <string>
#include <fstream>
#include <list>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <chrono>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <algorithm>
#include "library/bin.h"

namespace unitrade {

enum ThreadID {
  Current,
  FutCompute,
  FutExecute,
  StkCompute,
  StkExecute
};

enum SecurityType { STOCK, INDEX, FUTURES, ETF, BOND, SWAP, REPO, OPTION};

enum EventType {
  FuturesT, 
  OptionT,
  SwapT,
  StockT,
  IndexT,
  ETFT,
  BondT,
  RepoT
};


int FatalErrorMsg(const std::string msg);
int ErrorMsg(const std::string msg);
int WarningMsg(const std::string msg);
int Msg(const std::string sub, const std::string msg);


class Event;
class StkComputeEngine;
class StkExecuteEngine;
class FutComputeEngine;
class FutExecuteEngine;
class StockBook;
class StockOrderBook;
class StockConfigPara;
class FuturesConfigPara;
class Para;


class Condition {
 public:
  bool fut_quote_flag;
  bool fut_compute_flag;
  bool fut_execute_flag;
  bool stk_quote_flag;
  bool stk_compute_flag;
  bool stk_execute_flag;
  bool is_estimate_settlement;
  Condition() {
    fut_quote_flag = false;
    fut_compute_flag = false;
    fut_execute_flag = false;
    stk_quote_flag = false;
    stk_compute_flag = false;
    stk_execute_flag = false;
    is_estimate_settlement = false;
  }
  void Update(bin::Timer& clock, Para& para);
};


class Para {
 public:
  StockConfigPara* p_stockconfig;
  FuturesConfigPara* p_futuresconfig;
  bin::Timer start;
  bin::Timer exit_time;
  int year, month, day, day_of_month, day_of_week;
  std::string ACCOUNT;
  std::string stk_workspace;
  std::string fut_workspace;
  std::string workspace;
  std::string config;
  std::string shanghai_quote;
  std::string shenzhen_quote;
  std::string log;
  std::string stk_monitor;
  std::string fut_monitor;
  std::string task_monitor;
  std::string task_list;
  std::string extern_stkconfig;
  std::string extern_futconfig;
  std::string locker;
  std::string fut_compute;
  std::string fut_execute;
  std::string stk_compute;
  std::string stk_execute;
  std::string risk_management_list;
  std::string risk_monitor;
  std::string cash_socket;
  std::string command_socket;

  std::string error_log;
  std::string warning_log;

  std::string fund_account;
  std::string fund_passwd;
  std::string sh_account;
  std::string sz_account;

  std::string fut_account;
  std::string fut_passwd;

  std::string dbcn;

  std::string key_directory;
  std::string server_key_private;
  std::string server_key_public;
  std::string server_certificate;
  std::string client_key_public;

  int parse();
};


/* security is root trading securities*/
class security {
  public:
    std::string _name;
    std::string _code;
    SecurityType security_type;

};


/*Task is minimized task unit*/
class Task {
 public:
  std::string ID;
  std::string name;
  int status;  // -1 for null, 0 for successful, 1 for waiting, 2 for working, 3
               // for error
  virtual void start() {
    std::cout << "task starts, this should never happen" << std::endl;
  }
  virtual void execute() {
    std::cout << "task executes, this should never happen" << std::endl;
  }
  virtual void finalize() {
    std::cout << "task finishes, this should never happen" << std::endl;
  }
};


/*Event is an extension of Task*/
class Event : public Task {
 public:
  bin::Timer mark;
  bin::Timer beg;
  bin::Timer end;
  bin::TaskTimeType time_type;
  std::string des_status;
  std::string description;
  std::vector<std::shared_ptr<Event>> sub_events;
  bool is_leaf {false};
  bool is_threaded {false}; /*when work is in huge amount, needs to be threaded */
  std::shared_ptr<Event> next_event; /*should be defaultly constructed as
                                        nullptr*/
  Event* next_parent{nullptr};
  ThreadID thread_id{ThreadID::Current};
  std::string type;
  // bool need_execute;
  int try_times;
  int max_times;
  virtual void AssignParent(Event* p) { next_parent = p; }
  virtual void AssignNext(std::shared_ptr<Event> p) { next_event = p; }
  virtual void execute() {
    std::cout << "execute, this should never be called" << std::endl;
  };
  virtual void start() {
    std::cout << "start, this should never be called" << std::endl;
  };
  virtual void finalize() {
    std::cout << "finishes, this should never be called" << std::endl;
  };
  virtual void Inherit()
  {
    for (auto p:sub_events)
    {
      p->mark = mark; 
      p->time_type = time_type; 
      p->type = type;
    }
  };
  virtual void Spread()
  {
    for (auto p:sub_events)
    {
      p->des_status = des_status; 
    }
  };
 
  
  virtual void PrintToFile(std::ofstream& ofs) {

      ofs << std::setw(19) << name << std::setw(19) << name << std::setw(13)
          << (time_type == bin::TaskTimeType::OnStart
                  ? "OnStart"
                  : (time_type == bin::TaskTimeType::OnStop ? "OnStop"
                                                       : mark.printShortTime()))
          << std::setw(20) << des_status << std::setw(15)
          << beg.printShortTime() << std::setw(15) << end.printShortTime()
          << std::setw(16) << ID << std::endl;
    
      for (auto i:sub_events)
      {
        ofs << std::setw(19)<<" "<<std::setw(19) << i->name << std::setw(13)
          << (i->time_type == bin::TaskTimeType::OnStart
                  ? "OnStart"
                  : (i->time_type == bin::TaskTimeType::OnStop ? "OnStop"
                                                       : i->mark.printShortTime()))
          << std::setw(20) << i->des_status << std::setw(15)
          << i->beg.printShortTime() << std::setw(15) << i->end.printShortTime()
          << std::setw(16) << i->ID << std::endl;
     }
  }

  bool exclusive{false};
  Event(const std::string n);
};


/* specific events  */
class StockComputeEvent : public Event {
 public:
  StockComputeEvent(std::string n, StockBook* p,
                    std::shared_ptr<StockOrderBook> p_order,
                    std::shared_ptr<StkComputeEngine> p_com)
      : Event(n) {
    p_stockbook = p;
    p_orderbook = std::make_shared<StockOrderBook>(p);
    ID = "SC_" + bin::RandomString(5);
    p_orderbook = p_order;
    p_stkcomputeengine = p_com;
  }
  std::shared_ptr<StkComputeEngine> p_stkcomputeengine;
  std::shared_ptr<StockOrderBook> p_orderbook;
  StockBook* p_stockbook;
  bool need_execute{false};
  void execute();
  void start();
  void finalize();
};



class StockExecuteEvent : public Event {
 public:
  StockExecuteEvent(std::string n, StockBook* p,
                    std::shared_ptr<StockOrderBook> p_order,
                    std::shared_ptr<StkExecuteEngine> p_exe)
      : Event(n) {
    p_stockbook = p;
    p_orderbook = std::make_shared<StockOrderBook>(p);
    ID = "SE_" + bin::RandomString(5);
    p_orderbook = p_order;
    p_stkexecuteengine = p_exe;
  }
  std::shared_ptr<StkExecuteEngine> p_stkexecuteengine;
  std::shared_ptr<StockOrderBook> p_orderbook;
  StockBook* p_stockbook;
  bool need_execute{false};
  void start();
  void execute();
  void finalize();
};


class FuturesComputeEvent : public Event {
 public:
  FuturesComputeEvent(std::string n, std::shared_ptr<FutComputeEngine> p_exe)
      : Event(n) {
    ID = "FC_" + bin::RandomString(5);
    p_futcomputeengine = p_exe;
  }
  std::shared_ptr<FutComputeEngine> p_futcomputeengine;
  bool need_execute{false};
  void start();
  void execute();
  void finalize();
};


class FuturesExecuteEvent : public Event {
 public:
  FuturesExecuteEvent(std::string n, std::shared_ptr<FutExecuteEngine> p_exe)
      : Event(n) {
    ID = "FE_" + bin::RandomString(5);
    p_futexecuteengine = p_exe;
  }
  bool need_execute{false};
  void start();
  void execute();
  void finalize();
  std::shared_ptr<FutExecuteEngine> p_futexecuteengine;
};

/* Engines */
class StkComputeEngine {
 public:
  std::string name;
  bool need_execute;
  StkComputeEngine(const std::string n) {
    name = n;
    need_execute = false;
  }
  virtual void execute(std::shared_ptr<StockOrderBook> p_orderbook) {};
};

class StkExecuteEngine {
 public:
  std::string name;
  StkExecuteEngine(const std::string n) { name = n; }
  virtual void execute() {};
  virtual void execute(std::shared_ptr<StockOrderBook> p_orderbook) {};
};

class FutComputeEngine {
 public:
  std::string name;
  bool need_execute;
  FutComputeEngine(const std::string n) {
    name = n;
    need_execute = false;
  }
  virtual void execute() {};
};

class FutExecuteEngine {
 public:
  std::string name;
  FutExecuteEngine(const std::string n) { name = n; }
  virtual void execute() {};
};

class EventsByTime {
 public:
  Event* on_stk_compute_event;
  Event* on_stk_execute_event;
  Event* on_fut_compute_event;
  Event* on_fut_execute_event;
  void Register();
  typedef std::multimap<long, std::shared_ptr<Event>> EventList;
  EventList events;
  EventList event_schedule;
  std::shared_ptr<Event> on_event;
  std::vector<std::shared_ptr<Event>> OnStart;
  std::vector<std::shared_ptr<Event>> OnStop;

  void ExecuteOnStart() {
    for (unsigned int i = 0; i < OnStart.size(); ++i) {
      // on_event = OnStart[i];
      OnStart[i]->start();
      Msg("core", "OnStart->" + OnStart[i]->name + " executes, ID=" +
                      OnStart[i]->ID + ", [" + bin::printTime() + "]");
      // std::cout<<on_event->name<<std::endl;
    }
  }

  void ExecuteOnStop() {
    for (unsigned int i = 0; i < OnStop.size(); ++i) {
      // on_event = OnStop[i];
      OnStop[i]->start();
      Msg("core", "OnStop->" + OnStop[i]->name + " executes, ID=" +
                      OnStop[i]->ID + ", [" + bin::printTime() + "]");
    }
  }
  void Try();
  void ParseEvent(std::ifstream& ifs);
  void PrintToFile(std::ofstream& ofs) {
    ofs.seekp(0);
    EventList::iterator it;
    ofs << bin::printDate() << "\t" << bin::printTime() << std::endl;
    ofs << std::setw(19) << "NAME" << std::setw(19) << "SUB" << std::setw(13)
        << "SCHU" << std::setw(20) << "STATUS" << std::setw(15) << "START"
        << std::setw(15) << "END" << std::setw(16) << "ID" << std::endl;
    for (unsigned int i = 0; i < OnStart.size(); ++i) {
      OnStart[i]->PrintToFile(ofs);
    }
    for (it = events.begin(); it != events.end(); ++it) {
      (it)->second->PrintToFile(ofs);
    }
    for (unsigned int i = 0; i < OnStop.size(); ++i) {
      OnStop[i]->PrintToFile(ofs);
    }
  };
  int DeleteEvent(std::string id) {
    EventList::iterator it;
    for (it = events.begin(); it != events.end(); ++it) {
      if (it->second->ID == id) events.erase(it);
    }
    for (it = event_schedule.begin(); it != event_schedule.end(); ++it) {
      if (it->second->ID == id) event_schedule.erase(it);
    }
    return 0;
  }
};


class Daemon {
  public:

    int Triger(EventType type)
    {
      if (type==EventType::StockT) for(auto i:StockEvents) i->start();
      if (type==EventType::FuturesT) for(auto i:FuturesEvents) i->start();
      if (type==EventType::IndexT)  for(auto i:IndexEvents) i->start();
      if (type==EventType::ETFT)  for(auto i:ETFEvents) i->start();
      if (type==EventType::BondT)  for(auto i:BondEvents) i->start();
      if (type==EventType::SwapT)  for(auto i:SwapEvents) i->start();
      if (type==EventType::OptionT)  for(auto i:OptionEvents) i->start();
      return 0; 
    }

    int Register(std::shared_ptr<Event> p, EventType type)
    {
      if (type==EventType::StockT) StockEvents.push_back(p);
      if (type==EventType::FuturesT) FuturesEvents.push_back(p);
      if (type==EventType::IndexT) IndexEvents.push_back(p); 
      if (type==EventType::ETFT) ETFEvents.push_back(p); 
      if (type==EventType::BondT) BondEvents.push_back(p); 
      if (type==EventType::SwapT) SwapEvents.push_back(p); 
      if (type==EventType::OptionT) OptionEvents.push_back(p);  
      return 0;
    }  
  private:
    std::vector<std::shared_ptr<Event>> StockEvents;
    std::vector<std::shared_ptr<Event>> FuturesEvents;
    std::vector<std::shared_ptr<Event>> IndexEvents;
    std::vector<std::shared_ptr<Event>> SwapEvents;
    std::vector<std::shared_ptr<Event>> ETFEvents;
    std::vector<std::shared_ptr<Event>> OptionEvents;
    std::vector<std::shared_ptr<Event>> BondEvents;
};



// functions
//int getDate(Date& d);
std::shared_ptr<Event> EventFactory(std::vector<std::string>);
std::shared_ptr<StkExecuteEngine> StkExecuteEngineFactory(std::string);
std::shared_ptr<StkComputeEngine> StkComputeEngineFactory(std::string);
std::shared_ptr<FutExecuteEngine> FutExecuteEngineFactory(std::string);
std::shared_ptr<FutComputeEngine> FutComputeEngineFactory(std::string);
}
#endif
