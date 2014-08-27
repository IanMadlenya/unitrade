//
//  FutHeader.h
//  InventoryManagment
//
//  Created by Bin Fan on 7/2/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef InventoryManagment_Fut_Header_h
#define InventoryManagment_Fut_Header_h

#include "ComHeader.h"
#include <iostream>
#include "library/bin.h"

namespace unitrade {

void call_from_thread_fut_execute(const std::string name);
void call_from_thread_fut_compute(const std::string name);
void call_from_thread_fut_quote(const std::string name);

int FutureRound(double num, double &left, double add, bool isPush,
                double carryover);

typedef struct {
  double r1 = 0.5;
  double r2 = 0.5;
  double r3 = 0;
  std::string record;
  bool verify() {
    if (r1 + r2 + r3 == 1)
      return true;
    else
      return false;
  }
  std::string print() {
    record = bin::int2string(r1 * 100) + ":" + bin::int2string(r2 * 100) + ":" +
             bin::int2string(r3 * 100);
    return record;
  }
  void update(int ETA, int month) {
    if (ETA == 0) {
      r1 = 0;
      r2 = r3 = 0.5;
    } else if (ETA == -1) {
      if (month == 1 || month == 4 || month == 7 || month == 10) {
        r1 = r2;
        r2 = r3;
        r3 = 0;
      } else {
        r1 = r2;
        r2 = 0;
      }
    }
  }
} Ratio;

class FuturesConfigPara {
 public:
  Ratio ratio;
  int Year;
  int Month;
  int Day;
  int DayOfMonth;
  int DayOfWeek;
  int HEDGE_MODE;

  float StockCashUpperLine;
  float StockCashLowerLine;
  // double FuturesCashRedLine;
  // double FuturesCashYellowLine;
  float MarginRedLine;
  float MeltDown;
  float SettleMeltDown;
  float SettleFeeRate;
  float OpenFeeRate;
  float CloseTodayFeeRate;
  float CloseYesterdayFeeRate;
  float Beta;
  float RedLine;
  float BlueLine;
  float YellowLine;

  bin::Timer StockTime;
  bin::Timer CurrentTime;

  bin::Timer MarketStart;
  bin::Timer MarketEnd;

  bin::Timer NoonBreakStart;
  bin::Timer NoonBreakEnd;

  int TradeNum;

  bin::Timer EstimateSettleStart;
  bin::Timer EstimateSettleEnd;
  bin::Timer StockUpdateStart;

  std::string ACCOUNT;

  std::string fut_workspace;
  std::string futures_book;
  std::string dbcn;  // some external config file location, like cal.cn
  std::string HEDGE_FLAG;
  std::string PASSWORD;
  std::string INVESTOR_ID;
  std::string FRONT_ADDR;
  std::string FRONT_ADDR_TRADE;
  std::string BROKER_ID;
  std::string STOCK_FILE_NAME;
  std::string FILE_CONFIG;
  std::string FILE_TO_REPORT_ORDER;
  std::string FILE_TO_ORDER_DETAIL;
  std::string FILE_TO_ALL_ORDER;
  std::string FOLDER_TO_STRATEGY;
  std::string FILE_TO_RUNTIME;
  int NUM_INSTRUMENT;
  int ETA;
  std::vector<std::string> INSTRUMENT_NAME_LIST;
  bool isSettleDay;
  bool isTradeDay;
  bool needRoll;
  double CarryOver;
  bool TradeDoor;
  bool SettleDoor;
  bool isRollOver;
  bool isBE;
  int TailBit;  // should be 0, 1, or 2
  bool con;     //
  int parseParameter(std::ifstream &config);

  int SetETA();
  int decideIFNames();
  int setParameter() {
    TailBit = 0;
    needRoll = true;

    TradeNum = 0;
    isBE = false;
    con = false;
    // INSTRUMENT_NAME_LIST = (char **)malloc(sizeof(char*)*NUM_INSTRUMENT);
    struct tm *timeval;
    time_t tt;
    tt = time(NULL);
    timeval = localtime(&tt);
    DayOfWeek = timeval->tm_wday;
    DayOfMonth = timeval->tm_mday;
    Day = timeval->tm_yday;
    Month = timeval->tm_mon + 1;
    Year = timeval->tm_year + 1900;
    MeltDown = SettleMeltDown;
    if (SetETA() == -1) return -1;  // fatal error occurs
    decideIFNames();
    // STOCK_FILE_NAME = STOCK_DIR + int2string(Year)
    //+(int2string(Month).length()==1?"0"+int2string(Month):int2string(Month))
    //+(int2string(DayOfMonth).length()==1?"0"+int2string(DayOfMonth):int2string(DayOfMonth))+"_Smy.txt";
    ratio.update(ETA, Month);
    TradeNum++;
    return 0;
  }
};

class Futures {
 public:
  double bid1, bid2, bid3, bid4, bid5;
  double ask1, ask2, ask3, ask4, ask5;
  int bid1_vol, bid2_vol, bid3_vol, bid4_vol, bid5_vol;
  int ask1_vol, ask2_vol, ask3_vol, ask4_vol, ask5_vol;
  double LastPrice;  // last order price
  double PreSettlementPrice, SettlementPrice, EstimateSettlementPrice;
  double PreClosePrice;
  double value, basevalue;
  double OpenPrice;
  double HighPrice, LowPrice, AveragePrice;
  double PreOpenInterest, OpenInterest, OpenInterestChange;
  int PreVolume, Volume, VolumeChange;
  int EstimateVol;
  double Fee, TodayFee;
  double Turnover;
  int Pre_Pos;  // how many long orders we hold yesterday
  int check_pos;
  int Pos;          // how many orders we hold yesterday
  int Pos_Trading;  // how many orders are in trading process (could be
                    // negative)
  int Pos_Traded;   // how many orders are traded (could be negative)
  int Pre_Pos_Long;
  int Pos_Long;
  int ExpectPos;
  double ExpectPosLong;
  std::vector<int> InToday;  // in as positive, out as negative
  std::vector<double> InTodayPrice;
  double AV;
  double PL;
  double RealizedPL;
  double UnrealizedPL;
  double TotalPL;
  bool isSettleDay;
  std::string name;
  int hour, min, sec, milisec;  // last change time;
  int st_hour, st_min, st_sec,
      st_millisec;  // time associate with estimate settlemnt price

  int updatePL() {
    UnrealizedPL = Pos * (SettlementPrice - AV) * 300;
    PL = RealizedPL + UnrealizedPL;
    return 0;
  }
  int updateSettlementPrice() {
    double ratio;
    if (st_hour != hour || st_min != min || st_sec != sec ||
        st_millisec != milisec) {
      if (EstimateVol != 0)
        ratio = 1.0 * OpenInterestChange / (OpenInterestChange + EstimateVol);
      else
        ratio = 1;
      EstimateSettlementPrice =
          EstimateSettlementPrice * (1 - ratio) + LastPrice * ratio;
      EstimateVol = EstimateVol + OpenInterestChange;
      st_hour = hour;
      st_min = min;
      st_sec = sec;
      st_millisec = milisec;
      SettlementPrice = EstimateSettlementPrice;
    };

    return 0;
  }

  std::string printStampTime(bool is_full = true) {
    std::stringstream name;
    std::string h, m, s, ms;
    name << hour;
    h = name.str();
    name.str("");
    name << min;
    m = name.str();
    name.str("");
    name << sec;
    s = name.str();
    name.str("");
    name << milisec;
    ms = name.str();
    name.str("");
    if (is_full)
      return h + ":" + m + ":" + s + "." + ms;
    else
      return h + ":" + m + ":" + s;
  }

  std::string printRecord() {
    std::stringstream price;
    std::string t, p, bid, ask, bid_v, ask_v;
    t = printStampTime(false);
    price << LastPrice;
    p = price.str();
    price.str("");
    price << bid1;
    bid = price.str();
    price.str("");
    price << ask1;
    ask = price.str();
    price.str("");
    price << bid1_vol;
    bid_v = price.str();
    price.str("");
    price << ask1_vol;
    ask_v = price.str();
    price.str("");
    return t + "  " + "  " + p + "  " + bid + "  " + ask + "  " + bid_v + "  " +
           ask_v;
  }
  Futures(std::string _name) {
    name = _name;
    Pos = 0;
    Volume = PreVolume = VolumeChange = 0;
    check_pos = Pre_Pos = 0;
    AV = 0;
    PL = 0;
    UnrealizedPL = 0;
    RealizedPL = 0;
    TotalPL = 0;
    Pos_Long = 0;
    isSettleDay = 0;
    OpenInterest = 0;
    PreSettlementPrice = 0;
    EstimateVol = 0;
    EstimateSettlementPrice = 2000;
    SettlementPrice = 0;
    Fee = 0;
    TodayFee = 0;
    Pos_Trading = 0;
    Pos_Traded = 0;
    ExpectPos = 0;
  }

 private:
};

class FuturesBook {
 public:
  FuturesBook() {
    sqr_FFspread = 0;
    num_FFspread = 0;
    sum_FFspread = 0;
    index = 0;
    IFspread = 0;
    FFspread = 0;
    num = 0;
    todayfee = 0;
    RealizedPL = 0;
    fee = 0;
    bank_value = 0;
    cash = 0;
    pre_cash = 0;
    cashin = 0;
    expcash = 0;
    singal_num = 0;
    isLock = false;
    redeem_stk = 0;
    redeem_fut = 0;
    margin_rate = 0.12;
    PL = 0;
    Total_PL = 0;
    status = "RealTime";
  }
  double redeem_stk;
  double redeem_fut;
  int num;
  int total_pos;
  double avg_price;
  double transfer;
  int singal_num;
  double fetch;
  double bank_value;
  std::string status;
  std::vector<std::shared_ptr<Futures>> InstrumentList;
  std::map<std::string, std::shared_ptr<Futures>> instruments;
  void add(std::shared_ptr<Futures> pIns) {
    num++;
    instruments[pIns->name] = pIns;
    InstrumentList.push_back(pIns);
  }
  // class instrument** InstrumentList;
  // basevalue is previous day's settlement value
  // value is current value
  double IFspread, FFspread, index, num_FFspread, sqr_FFspread, sum_FFspread,
      avg_FFspread, std_FFspread;
  double basevalue, value, total_cost, e_value, o_value, cash, pre_cash, cost,
      cashin, expcash;
  double RealizedPL;
  double UnrealizedPL;
  double PL;
  double Total_PL;
  double CashRequest, StockRequest;
  double margin_rate;
  double margin, e_margin, o_margin;

  // the prefix e_ and o_ denotes "expected" and "official", respectively
  double fee, todayfee;
  bool con;
  bool isLock;
  std::string ACCOUNT;
  int ReadLocal(class Para &para);
  int PrintToBook(std::ofstream &WH) {
    // this is used to tell stock module the current
    // status of the futures part
    WH.seekp(0, std::ios::beg);
    WH.precision(2);
    WH.setf(std::ios::fixed);
    WH << std::setw(8) << bin::printDate() << std::setw(15) << bin::printTime()
       << std::setw(11) << ACCOUNT << std::setw(15) << status << std::endl;
    WH << std::setw(15) << "CASH" << std::setw(18) << "PREV_CASH"
       << std::setw(18) << "UNREAL_PL" << std::setw(18) << "REALZD_PL"
       << std::setw(18) << "PL" << std::setw(18) << "Total_PL" << std::setw(18)
       << "TRANSFER" << std::endl;
    WH << std::setw(15) << cash << std::setw(18) << pre_cash << std::setw(18)
       << UnrealizedPL << std::setw(18) << RealizedPL << std::setw(18) << PL
       << std::setw(18) << Total_PL << std::setw(18) << cashin << std::endl;
    WH << std::setw(15) << "TODAY_FEE" << std::setw(18) << "SETL_CASH"
       << std::setw(18) << "AVAL_CASH" << std::setw(18) << "OFL_AVAL_CASH"
       << std::endl;
    WH << std::setw(15) << todayfee << std::setw(18) << cash + UnrealizedPL
       << std::setw(18) << cash + UnrealizedPL - value * margin_rate
       << std::setw(18) << cash - value * margin_rate << std::endl;
    WH << std::setw(7) << "Ins" << std::setw(8) << "Pos" << std::setw(9)
       << "PrevPos" << std::setw(10) << "Price" << std::setw(10) << "EstSet"
       << std::setw(10) << "PrevSet" << std::setw(12) << "PL" << std::setw(12)
       << "RealPL" << std::setw(12) << "UnrealPL" << std::setw(12) << "TotalPL"
       << std::setw(10) << "AVCost" << std::setw(11) << "Fee" << std::setw(11)
       << "TotalFee" << std::endl;
    for (int i = 0; i < num; i++) {
      WH << std::setw(7) << InstrumentList[i]->name << std::setw(8)
         << InstrumentList[i]->Pos << std::setw(9) << InstrumentList[i]->Pre_Pos
         << std::setw(10) << InstrumentList[i]->LastPrice << std::setw(10)
         << InstrumentList[i]->EstimateSettlementPrice << std::setw(10)
         << InstrumentList[i]->PreSettlementPrice << std::setw(12)
         << InstrumentList[i]->PL << std::setw(12)
         << InstrumentList[i]->RealizedPL << std::setw(12)
         << InstrumentList[i]->UnrealizedPL << std::setw(12)
         << InstrumentList[i]->TotalPL << std::setw(10) << InstrumentList[i]->AV
         << std::setw(11) << InstrumentList[i]->TodayFee << std::setw(11)
         << InstrumentList[i]->Fee << std::endl;
    }
    return 0;
  };
  int updateSingal() {
    singal_num++;
    if (singal_num > 20) {
      num_FFspread++;
      FFspread = InstrumentList[1]->LastPrice - InstrumentList[0]->LastPrice;
      sum_FFspread += FFspread;
      sqr_FFspread += FFspread * FFspread;
      avg_FFspread = sum_FFspread / num_FFspread;
      std_FFspread = pow(
          sqr_FFspread / (num_FFspread - 1) - avg_FFspread * avg_FFspread, 0.5);
      IFspread = InstrumentList[0]->EstimateSettlementPrice - index;
    }
    return 0;
  };
  int updateFuturesBook(StockBook &sb, class FuturesConfigPara *config);
  void updateSettlementPrice() {
    for (int i = 0; i < num; i++) {
      InstrumentList[i]->updateSettlementPrice();
    }
  }
  int PrintToFile(std::ofstream &OF, class FuturesConfigPara *para) {
    OF.seekp(0, std::ios::beg);
    OF.setf(std::ios::fixed);
    OF.precision(2);

    std::string icon;
    icon = (para->TailBit == 100 ? "(L)" : "   ");
    using namespace std;
    OF << setw(12) << "Instrument:" << setw(13) << InstrumentList[0]->name
       << (para->TailBit == 0 ? "(T)" : icon) << setw(19)
       << InstrumentList[1]->name << (para->TailBit == 1 ? "(T)" : icon)
       << setw(19) << InstrumentList[2]->name
       << (para->TailBit == 2 ? "(T)" : icon) << endl;
    OF << setw(12) << "--------------------------------------------------------"
                      "--------------------" << endl;
    OF << setw(12) << "Ask:" << setw(13) << InstrumentList[0]->ask1 << setw(5)
       << InstrumentList[0]->ask1_vol << setw(17) << InstrumentList[1]->ask1
       << setw(5) << InstrumentList[1]->ask1_vol << setw(17)
       << InstrumentList[2]->ask1 << setw(5) << InstrumentList[2]->ask1_vol
       << endl;
    OF << setw(12) << "Last:" << setw(3) << "<--" << setw(10)
       << InstrumentList[0]->LastPrice << setw(5)
       << InstrumentList[0]->VolumeChange << setw(17)
       << InstrumentList[1]->LastPrice << setw(5)
       << InstrumentList[1]->VolumeChange << setw(17)
       << InstrumentList[2]->LastPrice << setw(5)
       << InstrumentList[2]->VolumeChange << endl;
    OF << setw(12) << "Bid:" << setw(13) << InstrumentList[0]->bid1 << setw(5)
       << InstrumentList[0]->bid1_vol << setw(17) << InstrumentList[1]->bid1
       << setw(5) << InstrumentList[1]->bid1_vol << setw(17)
       << InstrumentList[2]->bid1 << setw(5) << InstrumentList[2]->bid1_vol
       << endl;
    OF << setw(12) << "Change:" << setw(13)
       << InstrumentList[0]->LastPrice - InstrumentList[0]->PreSettlementPrice
       << setw(22)
       << InstrumentList[1]->LastPrice - InstrumentList[1]->PreSettlementPrice
       << setw(22)
       << InstrumentList[2]->LastPrice - InstrumentList[2]->PreSettlementPrice
       << endl;
    OF << setw(12) << "OpenInt:" << setw(13)
       << int(InstrumentList[0]->OpenInterest) << setw(22)
       << int(InstrumentList[1]->OpenInterest) << setw(22)
       << int(InstrumentList[2]->OpenInterest) << endl;
    OF << setw(12) << "Volume:" << setw(13) << InstrumentList[0]->Volume
       << setw(22) << InstrumentList[1]->Volume << setw(22)
       << InstrumentList[2]->Volume << endl;
    OF << setw(12) << "EstSettle:" << setw(13)
       << InstrumentList[0]->EstimateSettlementPrice << setw(22)
       << InstrumentList[1]->EstimateSettlementPrice << setw(22)
       << InstrumentList[2]->EstimateSettlementPrice << endl;
    OF << setw(12) << "Position:" << setw(13)
       << InstrumentList[0]->Pos + InstrumentList[0]->Pos_Trading << setw(22)
       << InstrumentList[1]->Pos + InstrumentList[1]->Pos_Trading << setw(22)
       << InstrumentList[2]->Pos + InstrumentList[2]->Pos_Trading << endl;
    OF << setw(9) << "Expect(" << (isLock ? "L" : "F") << "):" << setw(13)
       << InstrumentList[0]->ExpectPos << "(" << setw(7)
       << InstrumentList[0]->ExpectPosLong << ")" << setw(13)
       << InstrumentList[1]->ExpectPos << "(" << setw(7)
       << InstrumentList[1]->ExpectPosLong << ")" << setw(13)
       << InstrumentList[2]->ExpectPos << "(" << setw(7)
       << InstrumentList[2]->ExpectPosLong << ")" << endl;
    OF << setw(12) << "PL:" << setw(13) << InstrumentList[0]->PL << setw(22)
       << InstrumentList[1]->PL << setw(22) << InstrumentList[2]->PL << endl;
    OF << setw(12) << "Update:" << setw(13)
       << InstrumentList[0]->printStampTime() << setw(22)
       << InstrumentList[1]->printStampTime() << setw(22)
       << InstrumentList[2]->printStampTime() << std::endl;
    return 0;
  }

 private:
};

class FuturesOrder {
 public:
  bool TimeFlag;  // true: execute immediately
  int id;         //"start from 1, for identifying orders"
  std::string ID;
  int OriginalNumber;
  int Number;
  int TradeNumber;
  double LimitPrice;
  double TradePrice;
  std::string InstrumentName;
  bin::Timer schedule;

  // TThostFtdcExchangeIDType ExchangeID;
  // TThostFtdcOrderSysIDType OrderSysID; // order's ID in Exchange system
  // TThostFtdcRequestIDType RequestID; // order's ID in local program
  // TThostFtdcSessionIDType SessionID;
  // TThostFtdcFrontIDType FrontID;
  // TThostFtdcTimeType InitTime;   // when the order is placed
  // TThostFtdcTimeType FinishTime; // when the order is finished
  // TThostFtdcTimeType LastTime;   // the time for last status change

  std::string ExchangeID;
  std::string OrderSysID;  // order's ID in Exchange system
  int RequestID;           // order's ID in local program
  int SessionID;
  int FrontID;
  std::string InitTime;    // when the order is placed
  std::string FinishTime;  // when the order is finished
  std::string LastTime;    // the time for last status change

  std::shared_ptr<Futures> p_Futures;
  std::string OrderMode;  // "limit" for limite price or "market" for market
                          // trade
  std::string Offset;     // "close" or "open"
  std::string Direction;  // "sell" or "buy"
  bool isActive;
  // 0 for silent (not work, sleeping)
  // 1 for trading
  std::vector<int> TradedVolumeList;
  std::vector<double> TradedPriceList;
  std::vector<int> OrderRefList;

  int CTPError;
  int EXError;
  char CTPErrorString[256];
  char EXErrorString[256];

  int Status;
  // -3 "revoked"
  // -2 "canceled"
  // -1 "cancelling"
  // 0 "fail"
  // 1 "success"
  // 2 "partly traded and out of queue"
  // 3 "not traded and out of queue"
  // 4 "partly traded and in queue"
  // 5 "not traded and in queue"
  // 6 "sent"
  // 7 "prepared and ready"
  // 8 "silent" (no work, sleeping)
  // 50 "unknown"
  // 100 CTP error
  // 200 Exchange error

  FuturesOrder() {
    ID = "FutOrd-" + bin::RandomString(5);
    CTPError = 0;
    EXError = 0;
    TimeFlag = false;
    Number = 0;
    TradeNumber = 0;
    InstrumentName = "NONE";
    OrderMode = "NONE";
    Offset = "NONE";
    Direction = "NONE";  // InitTime=0;  LastTime=0;
    isActive = 0;
    Status = 5;
  }  // initialize Empty Order
  FuturesOrder(std::string name, std::string mode, std::string offset,
               std::string dir, int amount, double limitprice = 0) {
    TimeFlag = false;
    InstrumentName = name;
    OrderMode = mode;
    Offset = offset;
    Direction = dir;
    Number = amount;
    OriginalNumber = amount;
    TradeNumber = 0;
    InitTime = "000000";  // initial time would be 000000000
    Status = 7;
    LimitPrice = limitprice;
  }  // construct an order

  int updatePrice(class FuturesBook &onhand, int t) {
    for (unsigned int i = 0; i < onhand.InstrumentList.size(); i++) {
      if (onhand.InstrumentList[i]->name == InstrumentName) {
        if (Direction == "sell")
          LimitPrice = onhand.InstrumentList[i]->bid1 - 0.2 * t;
        else if (Direction == "buy")
          LimitPrice = onhand.InstrumentList[i]->ask1 + 0.2 * t;
      }
    }
    return 0;
  }

  std::string printOrder() {
    return InstrumentName + "--" + Offset + "--" +
           bin::int2string(OriginalNumber) + "--" + OrderMode;
  }
  std::string printStatus() {
    std::string final;
    switch (Status) {
      case(-3) :
        final = "revoked";
        break;
      case(-2) :
        final = "canceled";
        break;
      case(-1) :
        final = "canceling";
        break;
      case(0) :
        final = "fail";
        break;
      case(1) :
        final = "success";
        break;
      case(2) :
        final = "partial(out-que)";
        break;
      case(3) :
        final = "no-trade(out-que)";
        break;
      case(4) :
        final = "partial(in-que)";
        break;
      case(5) :
        final = "no-trade(in-que)";
        break;
      case(6) :
        final = "sent";
        break;
      case(7) :
        final = "ready";
        break;
      case(100) :
        final = "CTP-ERR";
        break;
      case(200) :
        final = "Exchange-ERR";
        break;
      case(50) :
        final = "UNKNOWN";
        break;
      default:
        final = "ERROR";
        break;
    }
    return final;
  }
};

class FuturesOrderBook {
 public:
  std::string ID;  // used for internal identification
  int Num;
  unsigned int pNew;
  int isActive;  // if any order is active: require discretion
  int isNew;     // if any new orders is inserted
  int num_success;
  int TryTimes;  // how many limit orders we will try for single
  int WaitTime;  // how long we will wait for any limte order
  int TotalTime;
  int unit;
  bool freeze;
  bool inTrading;
  bool WaitForCancel, ReadyForCancel, inCancel;
  unsigned long cnt_ActiveOrders;
  unsigned long cnt_FinishedOrders;
  unsigned long cnt_CancelledOrders;
  std::vector<std::shared_ptr<FuturesOrder>> AllOrders;  // used to store all
                                                      // orders
  bool need_execute {false};
  FuturesOrderBook() {
    Num = 0;  // initially no order
    num_success = 0;
    pNew = 0;  // initially start from first order
    cnt_ActiveOrders = cnt_FinishedOrders = cnt_CancelledOrders = 0;
    isActive = isNew = false;
    WaitForCancel = false;
    ReadyForCancel = false;
    inTrading = false;
    inCancel = false;
  }
  std::string PrintSummary() {
    return bin::int2string((int)AllOrders.size()) + " orders, " +
           bin::int2string((int)cnt_ActiveOrders) + " active";
  }
  int insertOrder(std::shared_ptr<FuturesOrder> order) {
    AllOrders.push_back(order);
    isActive = 1;  // double check
    isNew = 1;     // this announces new orders coming
    Num++;
    return 0;
  }

  int cancelOrder(unsigned int n) {
    if (n >= AllOrders.size() || n < pNew) return -1;
    std::shared_ptr<FuturesOrder> p = AllOrders[n];
    AllOrders.erase(AllOrders.begin() + n);
    return 0;
  }
  int cancelAllOrder() {
    std::shared_ptr<FuturesOrder> p;
    std::cerr << "--->>> Cancel-all-order: wait for trading to be frozen"
              << std::endl;
    std::cerr << "--->>> Cancel-all-order: " << AllOrders.size() - pNew
              << " order(s) to be cancelled " << std::endl;

    WaitForCancel = true;
    while (!ReadyForCancel || inTrading) {
      std::cerr << "--->>> Cancel-all-order: waiting for current order "
                   "processing to finish " << std::endl;
      sleep(1);
    }
    std::cerr << "--->>> Cancel-all-order: canceling orders" << std::endl;
    for (int i = pNew; pNew < AllOrders.size(); i++) {
      p = AllOrders[pNew];
      std::cerr << "--->>> Cancel-all-order: canceling order NO." << i + 1
                << std::endl;
      AllOrders.erase(AllOrders.begin() + pNew);
    }
    std::cerr << "--->>> Cancel-all-order: cancel accomplished" << std::endl;
    WaitForCancel = false;
    ReadyForCancel = false;
    return 0;
  }

  int printBatchOrder(std::ofstream &OF) {
    if (AllOrders.size() == 0)  // no order
      OF << " No Order" << std::endl;
    std::string show;
    unsigned long p_start, p_end;
    if (AllOrders.size() < 7) {
      p_start = 0;
      p_end = AllOrders.size() - 1;

    } else if (pNew < 3) {
      p_start = 0;
      p_end = 6;
    } else if (pNew > AllOrders.size() - 4) {
      p_end = AllOrders.size() - 1;
      p_start = p_end - 6;
      OF << "   ......      " << p_start << " orders not shown  " << num_success
         << " success " << pNew - num_success << " fail    ......."
         << std::endl;
    } else {
      p_start = pNew - 3;
      p_end = pNew + 3;
      OF << "   ......      " << p_start << " orders not shown, " << num_success
         << " success " << pNew - num_success << " fail    ......."
         << std::endl;
    }
    for (unsigned long i = p_start; i <= p_end; i++) {
      show = AllOrders[i]->Offset + "-" + AllOrders[i]->Direction;
      if (AllOrders[i]->OrderMode == "limit") {
        if (pNew == i)
          OF << "-->";
        else
          OF << "   ";
        OF << "ID:" << std::setw(3) << i + 1 << std::setw(7)
           << AllOrders[i]->InstrumentName << std::setw(13) << (show)
           << std::setw(4) << AllOrders[i]->OriginalNumber << std::setw(4)
           << AllOrders[i]->Number << std::setw(7) << "lim-"
           << AllOrders[i]->LimitPrice << std::setw(19)
           << AllOrders[i]->printStatus() << std::setw(12)
           << (AllOrders[i]->InitTime) << std::endl;
      } else {
        if (pNew == i)
          OF << "-->";
        else
          OF << "   ";
        OF << "ID:" << std::setw(3) << i + 1 << std::setw(7)
           << AllOrders[i]->InstrumentName << std::setw(13) << (show)
           << std::setw(4) << AllOrders[i]->OriginalNumber << std::setw(4)
           << AllOrders[i]->Number << std::setw(14) << "market" << std::setw(19)
           << AllOrders[i]->printStatus() << std::setw(12)
           << (AllOrders[i]->InitTime) << std::endl;
      }
    }
    if (p_end < AllOrders.size() - 1)
      OF << "   ......      " << AllOrders.size() - p_end - 1
         << " prepared orders not shown     ......." << std::endl;
    return 0;
  }
  int printBatchOrderToScreen() {
    if (AllOrders.size() == 0)  // no order
      std::cout << " No Order" << std::endl;
    std::string show;
    unsigned long p_start, p_end;
    /*
     if (pool.size()<7)
     {
     p_start = 0;
     p_end = pool.size()-1;

     }
     else if (pNew<3)
     {
     p_start=0;
     p_end=6;
     }
     else if (pNew>pool.size()-4)
     {
     p_end=pool.size()-1;
     p_start = p_end-6;
     cout<<"   ......      "<<p_start<< " orders not shown     ......."<<endl;
     }
     else
     {
     cout<<"   ......      "<<p_start<< " orders not shown     ......."<<endl;
     p_start=pNew-3;
     p_end=pNew+3;
     }
     */
    p_start = 0;
    p_end = AllOrders.size() - 1;
    for (unsigned long i = p_start; i <= p_end; i++) {
      show = AllOrders[i]->Offset + "-" + AllOrders[i]->Direction;
      if (AllOrders[i]->OrderMode == "limit") {
        if (pNew == i)
          std::cout << "-->";
        else
          std::cout << "   ";
        std::cout << "ID:" << std::setw(3) << i + 1 << std::setw(7)
                  << AllOrders[i]->InstrumentName << std::setw(13) << (show)
                  << std::setw(4) << AllOrders[i]->OriginalNumber
                  << std::setw(4) << AllOrders[i]->Number << std::setw(7)
                  << "lim-" << AllOrders[i]->LimitPrice << std::setw(19)
                  << AllOrders[i]->printStatus() << std::setw(12)
                  << (AllOrders[i]->InitTime) << std::endl;
      } else {
        if (pNew == i)
          std::cout << "-->";
        else
          std::cout << "   ";
        std::cout << "ID:" << std::setw(3) << i + 1 << std::setw(7)
                  << AllOrders[i]->InstrumentName << std::setw(13) << (show)
                  << std::setw(4) << AllOrders[i]->OriginalNumber
                  << std::setw(4) << AllOrders[i]->Number << std::setw(14)
                  << "market" << std::setw(19) << AllOrders[i]->printStatus()
                  << std::setw(12) << (AllOrders[i]->InitTime) << std::endl;
      }
    }
    if (p_end < AllOrders.size() - 1)
      std::cout << "   ......      " << AllOrders.size() - p_end - 1
                << " orders not shown     ......." << std::endl;
    return 0;
  }

  int recordBatchOrder(std::ofstream &OF) {
    OF.seekp(std::ios::beg);
    OF << std::setw(10) << "REF_ID" << std::setw(15) << "Instrument"
       << std::setw(15) << "Direction" << std::setw(15) << "Type"
       << std::setw(15) << "Volumn" << std::setw(15) << "Status"
       << std::setw(15) << "StartTime" << std::setw(15) << "FinishedTime"
       << std::endl;
    if (AllOrders.size() == 0)  // no order
      std::cout << "NONE\t\t"
                << "NONE\t\t"
                << "NONE\t\t"
                << "NONE\t\t"
                << "NONE\t\t"
                << "NONE" << std::endl;
    else {
      for (unsigned int i = 0; i < AllOrders.size(); i++) {
        OF << std::setw(10) << AllOrders[i]->ID << std::setw(8)
           << AllOrders[i]->InstrumentName << std::setw(8)
           << (AllOrders[i]->Offset == "open" ? "open" : "close")
           << std::setw(8) << AllOrders[i]->OrderMode << std::setw(8)
           << AllOrders[i]->Number << std::setw(15)
           << AllOrders[i]->printStatus() << std::setw(10)
           << AllOrders[i]->InitTime << std::setw(10)
           << AllOrders[i]->FinishTime << std::endl;
      }
    }
    return 0;
  }
};

class FuturesTradeEvent : public Event {
 public:
  FuturesTradeEvent(std::string n, shared_ptr<FutComputeEngine> p_com_eng,
                    shared_ptr<FutExecuteEngine> p_exe_eng)
      : Event(n) {
    ID = "FT_" + bin::RandomString(5);

    auto p1 =  make_shared<FuturesComputeEvent>("FuturesCompute", p_com_eng); 
    auto p2 =  make_shared<FuturesExecuteEvent>("FuturesExecute", p_exe_eng);
    p1->AssignNext(p2);
    p2->AssignParent(this);
    p1->thread_id = ThreadID::FutCompute;
    p2->thread_id = ThreadID::FutExecute;
    sub_events.push_back(p1);
    sub_events.push_back(p2);
    
   
  }
  // std::vector<shared_ptr<Event>> sub_events;
  // shared_ptr<Event> next_event;
  // shared_ptr<Event> parent_event;
  virtual void start();
  virtual void execute();
  virtual void finalize();
};

class QueryFuturesEvent : public Event {
 public:
  QueryFuturesEvent(std::string n, shared_ptr<FutExecuteEngine> p_exe)
      : Event(n) {
    ID = "QF_" + bin::RandomString(5);
    p_futexecuteengine = p_exe;
  }
  void start();
  void execute();
  void finalize();
  shared_ptr<FutExecuteEngine> p_futexecuteengine;
};

class QueryFuturesSettlementEvent : public Event {
 public:
  QueryFuturesSettlementEvent(std::string n, shared_ptr<FutExecuteEngine> p_exe)
      : Event(n) {
    ID = "QFS_" + bin::RandomString(5);
    p_futexecuteengine = p_exe;
  }
  void start();
  void execute();
  void finalize();
  shared_ptr<FutExecuteEngine> p_futexecuteengine;
};

int FuturesBookFactory(FuturesBook &sb, FuturesConfigPara &para,
                       bin::Timer &wallclock);
}
#endif
