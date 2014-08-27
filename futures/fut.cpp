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
#include "FutHeader.h"

#include <fstream>

using namespace std;

namespace unitrade
{

int FutureRound(double num, double& left, double add, bool isPush, double carryover){
    int sign;
    num += add;
   	sign = (num>=0?1:-1);
	num=fabs(num);
	left = num - floor(num);
	left *= sign;
	if (isPush){
        if (fabs(left) > carryover)
		{
            return sign*(floor(num)+1);
		}
		else
		{
            return  sign*floor(num);
		}
	}
	else
	{
		return sign*floor(num);
	}
	return -1;
}

int FuturesBookFactory(FuturesBook& fb, FuturesConfigPara& para, bin::Timer& wallclock){

    for (int i=0; i<para.NUM_INSTRUMENT; ++i)
    {
        fb.add(make_shared<Futures>(para.INSTRUMENT_NAME_LIST[i]));
    }
    fb.ACCOUNT = para.ACCOUNT;
    fb.margin_rate = para.MarginRedLine;
    return 0;
}

int FuturesConfigPara::decideIFNames(){
    std::stringstream mout;
    std::stringstream yout;
    string ID1, ID2, ID3, ID4;
    string Pre="IF";
    int m = Month;
    int y = Year;
    //--------------------------
    //check if passed current month's settlement day
    if (ETA<0)
    {
        y = ( (m+1)>12?(y+1):y );
        m = ( (m+1)%12==0?12:(m+1)%12 );
    }
    //--------------------------
    
    
    // ID# represent the name of # instrument
    mout <<setfill('0') << setw(2)<< m;  //setfill('0') bz month needs padding
    yout <<setfill('0') << setw(2)<< y-2000;
    ID1 = Pre+yout.str()+mout.str();
    mout.str("");
    yout.str("");
    mout <<setfill('0') << setw(2)<<  ((m+1) %12 ==0?12:(m+1)%12);
    yout <<setfill('0') << setw(2)<< (m+1>=13?(y+1-2000):(y-2000));
    ID2 = Pre+yout.str()+mout.str();
    mout.str("");
    yout.str("");
    if (m%3 == 0)
    {
        mout<<setfill('0') << setw(2)<< ((m+3) %12==0?12:(m+3)%12);
        yout<<setfill('0') << setw(2)<< (m+3>=13?(y+1-2000):(y-2000));
        ID3 = Pre+yout.str()+mout.str();
        mout.str("");
        yout.str("");
        mout<<setfill('0') << setw(2)<< ((m+6) %12==0?12:(m+6)%12);
        yout<<setfill('0') << setw(2)<< (m+6>=13?(y+1-2000):(y-2000));
        ID4 = Pre+yout.str()+mout.str();
        mout.str("");
        yout.str("");
    }
    else if (m%3 == 1){
        mout<<setfill('0') << setw(2)<< ((m+2) %12==0?12:(m+2)%12);
        yout<<setfill('0') << setw(2)<< (m+2>=13?(y+1-2000):(y-2000));
        ID3 = Pre+yout.str()+mout.str();
        con=true;
        mout.str("");
        yout.str("");
        mout<<setfill('0') << setw(2)<< ((m+5) %12==0?12:(m+5)%12);
        yout<<setfill('0') << setw(2)<< (m+5>=13?(y+1-2000):(y-2000));
        ID4 = Pre+yout.str()+mout.str();
        mout.str("");
        yout.str("");
    }
    
    else if (m%3 == 2){
        mout<<setfill('0') << setw(2)<< ((m+4) %12==0?12:(m+4)%12);
        yout<<setfill('0') << setw(2)<< (m+4>=13?(y+1-2000):(y-2000));
        ID3 = Pre+yout.str()+mout.str();
        mout.str("");
        yout.str("");
        
        mout<<setfill('0') << setw(2)<< ((m+7) %12==0?12:(m+7)%12);
        yout<<setfill('0') << setw(2)<< (m+7>=13?(y+1-2000):(y-2000));
        ID4 = Pre+yout.str()+mout.str();
        mout.str("");
        yout.str("");
    }
    
    INSTRUMENT_NAME_LIST.push_back(ID1);
    INSTRUMENT_NAME_LIST.push_back(ID2);
    INSTRUMENT_NAME_LIST.push_back(ID3);
    NUM_INSTRUMENT = 3;
    
    return 0;
}







int FuturesConfigPara::parseParameter(ifstream& config){
    //parse all parameters from config file
    string spara;
    string ss;
    
    bin::ParseConfig("FUTURES_BOOK", futures_book ,config);
    futures_book = bin::FolderAppend(fut_workspace, futures_book);
    Msg("core", "set futures_book="+futures_book);
    
    bin::ParseConfig("FUTURES_REPORT_ORDER", FILE_TO_REPORT_ORDER, config);
    FILE_TO_REPORT_ORDER = bin::FolderAppend(fut_workspace, FILE_TO_REPORT_ORDER);
    Msg("core", "set futures report order="+FILE_TO_REPORT_ORDER);

    
    bin::ParseConfig("FUTURES_ORDER_DETAIL",FILE_TO_ORDER_DETAIL, config );
    FILE_TO_ORDER_DETAIL = bin::FolderAppend(fut_workspace, FILE_TO_ORDER_DETAIL);
    Msg("core", "set futures order detail="+FILE_TO_ORDER_DETAIL);

    bin::ParseConfig("FOLDER_TO_STRATEGY",FOLDER_TO_STRATEGY, config);
    FOLDER_TO_STRATEGY = bin::FolderAppend(fut_workspace, FOLDER_TO_STRATEGY);
    Msg("core", "set folder to strategy="+FOLDER_TO_STRATEGY);

    
    bin::ParseConfig("FILE_TO_RUNTIME", FILE_TO_RUNTIME, config);
    FILE_TO_RUNTIME =  bin::FolderAppend(fut_workspace, FILE_TO_RUNTIME);
    Msg("core", "set fut runtime="+FILE_TO_RUNTIME);

    
    bin::ParseConfig("INVESTOR_ID", INVESTOR_ID, config);
    Msg("core", "set INVESTOR_ID="+INVESTOR_ID);

    bin::ParseConfig("BROKER_ID", BROKER_ID, config);
    bin::ParseConfig("PASSWORD", PASSWORD, config);
    Msg("core", "set FUTURES PASSWD");

    bin::ParseConfig("FRONT_ADDR", FRONT_ADDR, config);
    bin::ParseConfig("FRONT_ADDR_TRADE", FRONT_ADDR_TRADE, config);
    
    bin::ParseConfig("HEDGE_FLAG", HEDGE_FLAG, config);
    bin::ParseConfig("NUM_INSTRUMENT",spara, config );
    NUM_INSTRUMENT=atoi(spara.c_str());
    
    

    bin::ParseConfig("STOCK_UPDATE_START_TIME", spara, config);
    StockUpdateStart.set(spara);
    bin::ParseConfig("MARKET_START_TIME", spara, config );
    MarketStart.set(spara);
    bin::ParseConfig("MARKET_END_TIME", spara, config );
    MarketEnd.set(spara);
    Msg("core", "Fut Market Time "+MarketStart.printShortTime()+"->"+MarketEnd.printShortTime());
    bin::ParseConfig("NOON_BREAK_START_TIME", spara, config );
    NoonBreakStart.set(spara);
    bin::ParseConfig("NOON_BREAK_END_TIME", spara, config );
    NoonBreakEnd.set(spara);
    Msg("core", "Fut Noon Break "+NoonBreakStart.printShortTime()+"->"+NoonBreakEnd.printShortTime());

    bin::ParseConfig("ESTIMATE_SETTLE_START_TIME", spara, config);
    EstimateSettleStart.set(spara);
    bin::ParseConfig("ESTIMATE_SETTLE_END_TIME", spara, config);
    EstimateSettleEnd.set(spara);

    
    
    HEDGE_MODE = 0;
    bin::ParseConfig("HEDGE_MODE", HEDGE_MODE ,config);
    
    bin::ParseConfig("STOCK_CASH_UPPER_LINE", StockCashUpperLine, config);
  
    bin::ParseConfig("STOCK_CASH_LOWER_LINE", StockCashLowerLine, config);
   
    bin::ParseConfig("SETTLE_FEE_RATE", SettleFeeRate, config);

    bin::ParseConfig("OPEN_FEE_RATE", OpenFeeRate, config);

    bin::ParseConfig("CLOSE_TODAY_FEE_RATE", CloseTodayFeeRate, config);

    bin::ParseConfig("CLOSE_YESTERDAY_FEE_RATE", CloseYesterdayFeeRate, config);

    
    
    // check some parameters
    if (setParameter()==-1)
        return -1;
    return 0;
};



int FuturesConfigPara::SetETA(){
    Msg("core", "processing futures trading day info");
    std::vector<int> trading_day_list;
    std::ifstream IF;
    std::string cal, line;
    cal=bin::FolderAppend(dbcn, "cal_cn.txt");
    if(!bin::isFileExist(cal))
    {
        FatalErrorMsg("cal file does not exist!"+cal);
        stk_exit = true;
        fut_exit = true;
        return -1;
    }
    IF.open(cal.c_str());
    while (IF.good())
    {
        getline(IF, line);
        trading_day_list.push_back(atoi(line.c_str()));
    }
    IF.close();
    int DayOfMon = DayOfMonth;
    
    string s_today;
    int i_today;
    
    s_today+=bin::int2string(Year);
    line=(Month<10?"0"+bin::int2string(Month):bin::int2string(Month));
    s_today+=line;
    line=(DayOfMon<10?"0"+bin::int2string(DayOfMon):bin::int2string(DayOfMon));
    s_today+=line;
    i_today=atoi(s_today.c_str());
    
    std::vector<int>::iterator up;
    
    if(std::find(trading_day_list.begin(), trading_day_list.end(),i_today)!=trading_day_list.end())
    {
        isTradeDay = true;
        cerr<<"--->>> core: today is trading day"<<endl;
    }
    else
    {
        isTradeDay = false;
        cerr<<"--->>> core: today is *not* trading day"<<endl;
        up= std::upper_bound (trading_day_list.begin(), trading_day_list.end(), i_today);
        cerr<<"--->>> core: nearest trading day is: "<<trading_day_list[up-trading_day_list.begin()]<<endl;
    }
    
    
    int incre;
    incre = (DayOfWeek == 6?6:(5-DayOfWeek));
    //DayOfWeek += incre;
    DayOfMon += incre;
    if (DayOfMon-14>0 && DayOfMon-14<8)
    {
        ETA=incre;
    }
    else if (DayOfMon-7>0 && DayOfMon-7<8)
    {
        ETA=incre+7;
    }
    else if (DayOfMon>0 && DayOfMon<8)
    {
        ETA=incre+14;
    }
    else if (DayOfMon-21>0 && DayOfMon-21<8)
    {
        ETA=incre-7;
    }
    else if (DayOfMon-28>0 && DayOfMon-28<8)
    {
        ETA=incre-14;
    }
    int i_settle = i_today+ETA;
    cerr<<"--->>> core: settle day should be "<<i_settle<<endl;
    if(std::find(trading_day_list.begin(), trading_day_list.end(),i_settle)==trading_day_list.end())
    {
        up= std::upper_bound (trading_day_list.begin(), trading_day_list.end(), i_settle);
        i_settle = trading_day_list[up-trading_day_list.begin()];
        cerr<<"--->>> core: scheduled settle day is not trading day, thus it is postponed to "<<i_settle<<endl;
    }
    if(i_settle==i_today)
    {
        isSettleDay = true;
        ETA=0;
        cerr<<"--->>> core: today is settle day, ETA="<<ETA<<endl;
    }
    else
    {
        isSettleDay = false;
        ETA = (int) (find(trading_day_list.begin(), trading_day_list.end(),i_settle)-find(trading_day_list.begin(), trading_day_list.end(),i_today));
        cerr<<"--->>> core: today is not settle day, ETA="<<ETA<<endl;
    }
    
    return 0;
}






int FuturesBook::ReadLocal(class Para &para){
    ifstream F_settlement;
    string date;
    bool fresh=true;
    int y, m, d;
    string line;

    vector<string> vs;
    if (bin::isFileEmpty(para.p_futuresconfig->futures_book))
    {
        WarningMsg("local futures book is empty, proceed anyway");
        return 1;
    }
    
    
    F_settlement.open(para.p_futuresconfig->futures_book);
    if (!F_settlement.is_open()){
        WarningMsg("cannot open futures book, proceed anyway");
        return 1;
    }
    
    stringstream stream;
    getline(F_settlement, line);
    
    stringstream ss(line);
    ss << line;
    ss >> date;

    bin::ParseDate(date,y,m,d);
    if (y==para.year && m==para.month && d==para.day_of_month)
    {
        fresh=false;
        WarningMsg("this is *not* a fresh start");
    }
    else
    {
        fresh=true;
        Msg("core", "this is a fresh start") ;
    }
    getline(F_settlement, line);
    if (F_settlement.good())
        getline(F_settlement, line);
    else
        return 1;
    
    vs = bin::SplitLine(line);
    if (vs.size()!=7)
    {
        WarningMsg("local futures book format not compatible");
        WarningMsg(line);
        return 1; 
    }
    cash = atof(vs[0].c_str());
    if (y==para.year && m==para.month && d==para.day_of_month)
    {
        pre_cash = atof(vs[1].c_str());//atof(scashin.c_str());
        cashin = atof(vs[6].c_str());

    }
    else
    {
        pre_cash = atof(vs[0].c_str());//atof(scashin.c_str());
        cashin=0;
    }
    if (F_settlement.good())
        getline(F_settlement, line);
    else
        return 1;
    if (F_settlement.good())
        getline(F_settlement, line);
    else
        return 1;
    if (F_settlement.good())
        getline(F_settlement, line);
    else
        return 1;

    
    while (getline(F_settlement, line))
    {
        //cout<<amount<<endl;
        vs = bin::SplitLine(line);
        if (vs.size()!=13)
        {
            WarningMsg("local futures book format not compatible");
            WarningMsg(line);
            continue;
        }
        for (unsigned int i=0; i<InstrumentList.size(); i++){
            //cout<<price<<endl;
            if (vs[0] == InstrumentList[i]->name)
            {
                InstrumentList[i]->Pos = atoi(vs[1].c_str());
                InstrumentList[i]->AV = atof(vs[10].c_str());
                if (!fresh)
                {
                    InstrumentList[i]->Pre_Pos = atoi(vs[2].c_str());
                    InstrumentList[i]->PreSettlementPrice = atof(vs[5].c_str());
                    InstrumentList[i]->PL = atof(vs[6].c_str());
                    InstrumentList[i]->RealizedPL = atof(vs[7].c_str());
                    InstrumentList[i]->UnrealizedPL = atof(vs[8].c_str());
                    InstrumentList[i]->TodayFee = atof(vs[11].c_str());
                }
                else
                {
                    InstrumentList[i]->Pre_Pos = atoi(vs[1].c_str());
                    InstrumentList[i]->PreSettlementPrice = atof(vs[3].c_str());
                    InstrumentList[i]->PL = 0;
                    InstrumentList[i]->RealizedPL = 0;
                    InstrumentList[i]->UnrealizedPL = 0 ;
                    InstrumentList[i]->TodayFee = 0;
                }
                InstrumentList[i]->TotalPL = atof(vs[9].c_str());
                InstrumentList[i]->Fee = atof(vs[12].c_str());
            }
        }
    }
    
    cerr<<"--->>> According to record, we hold contracts:"<<endl;
    cerr<<"     "<<"Instrument\t\tSettlement\tAmount"<<endl;
    for (unsigned int i=0; i<InstrumentList.size(); i++){
        cerr<<"--->>> "<<InstrumentList[i]->name<<"\t\t"
        <<InstrumentList[i]->PreSettlementPrice<<"\t\t"
        <<InstrumentList[i]->Pre_Pos<<endl;
    }
    cerr<<fixed;
    cerr<<setprecision(2);
    cerr<<"--->>> Total Cash: "<<cash<<endl;
    F_settlement.close();
    return 0;
}


int FuturesBook::updateFuturesBook(StockBook &sb, class FuturesConfigPara *config){
    basevalue = 0;
    fee = 0;
    todayfee = 0;
    UnrealizedPL = 0;
    RealizedPL = 0;
    total_cost=0;
    total_pos=0;
    avg_price=0;
    double fleft1, fleft2,fleft3;
    fleft1=fleft2=fleft3=0;
    for (int i=0; i<num; i++)
    {
        //total_pos += abs(InstrumentList[i]->Pos);
        //avg_price += InstrumentList[i]->LastPrice*abs(InstrumentList[i]->Pos);
        //total_cost += InstrumentList[i]->AV*abs(InstrumentList[i]->Pos)*300;
        fee += InstrumentList[i]->Fee;
        todayfee += InstrumentList[i]->TodayFee;
        UnrealizedPL += InstrumentList[i]->UnrealizedPL;
        RealizedPL += InstrumentList[i]->RealizedPL;
        PL += InstrumentList[i]->PL;
        Total_PL += InstrumentList[i]->TotalPL;
    }
    //avg_price /= total_pos;
    value = 0;
    e_value=0;
    o_value=0;
    for (int i=0; i<num; i++){
        value += fabs(InstrumentList[i]->Pos)*InstrumentList[i]->LastPrice*300;
        e_value += fabs(InstrumentList[i]->ExpectPos)*InstrumentList[i]->LastPrice*300;
        o_value += fabs(InstrumentList[i]->Pos)*InstrumentList[i]->PreSettlementPrice*300;
    }
    margin = (value==0?1:(cash+UnrealizedPL)/value);
    e_margin = (e_value==0?1:(cash+UnrealizedPL)/e_value);
    o_margin = (o_value==0?1:(cash+UnrealizedPL)/o_value);
    //fetch=(UnrealizedPL>=0?(cash-o_value*config->MarginRedLine):(cash+UnrealizedPL-o_value*config->MarginRedLine));
    //StockRequest = 0;
    if (!isLock)
    {
        if (config->TailBit==1)
        {
            //cout<<fleft1<<"\t"<<fleft2<<endl;
            InstrumentList[2]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r3)*(-1)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice), fleft2, 0, false, config->CarryOver);
            InstrumentList[2]->ExpectPosLong = sb.market_value*(config->ratio.r3)*(-1.0)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice);
            
            
            InstrumentList[0]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r1)*(-1)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice), fleft1, 0, false, config->CarryOver);
            InstrumentList[0]->ExpectPosLong = sb.market_value*(config->ratio.r1)*(-1.0)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice);
            InstrumentList[1]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r2)*(-1)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice), fleft3, fleft1+fleft2, true, config->CarryOver);
            InstrumentList[1]->ExpectPosLong = sb.market_value*(config->ratio.r2)*(-1.0)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice);
            
        }
        else if (config->TailBit==0)
        {
            InstrumentList[2]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r3)*(-1)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice), fleft2, 0, false, config->CarryOver);
            InstrumentList[2]->ExpectPosLong = sb.market_value*(config->ratio.r3)*(-1.0)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice);
            InstrumentList[1]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r2)*(-1)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice), fleft3, 0, false, config->CarryOver);
            InstrumentList[1]->ExpectPosLong = sb.market_value*(config->ratio.r2)*(-1.0)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice);
            InstrumentList[0]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r1)*(-1)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice), fleft1, fleft2+fleft3, true, config->CarryOver);
            InstrumentList[0]->ExpectPosLong = sb.market_value*(config->ratio.r1)*(-1.0)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice);
        }
        else if (config->TailBit==2)
        {
            InstrumentList[0]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r1)*(-1)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice), fleft2, 0, false, config->CarryOver);
            InstrumentList[0]->ExpectPosLong = sb.market_value*(config->ratio.r1)*(-1.0)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice);
            InstrumentList[1]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r2)*(-1)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice), fleft3, 0, false, config->CarryOver);
            InstrumentList[1]->ExpectPosLong = sb.market_value*(config->ratio.r2)*(-1.0)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice);
            InstrumentList[2]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r3)*(-1)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice), fleft1, fleft2+fleft3, true, config->CarryOver);
            InstrumentList[2]->ExpectPosLong = sb.market_value*(config->ratio.r3)*(-1.0)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice);
        }
        else if (config->TailBit==100)
        {
            InstrumentList[0]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r1)*(-1)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice), fleft2, 0, true, config->CarryOver);
            InstrumentList[0]->ExpectPosLong = sb.market_value*(config->ratio.r1)*(-1.0)*config->Beta/(300*InstrumentList[0]->EstimateSettlementPrice);
            InstrumentList[1]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r2)*(-1)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice), fleft3, 0, true, config->CarryOver);
            InstrumentList[1]->ExpectPosLong = sb.market_value*(config->ratio.r2)*(-1.0)*config->Beta/(300*InstrumentList[1]->EstimateSettlementPrice);
            InstrumentList[2]->ExpectPos = FutureRound(sb.market_value*(config->ratio.r3)*(-1)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice), fleft1, 0, true, config->CarryOver);
            InstrumentList[2]->ExpectPosLong = sb.market_value*(config->ratio.r3)*(-1.0)*config->Beta/(300*InstrumentList[2]->EstimateSettlementPrice);
        }
    }
    return 0;
    
};



void QueryFuturesEvent::start()
{
    beg = wall_clock;
    des_status="active";
    Msg("core", "QueryFutures executes");
    std::unique_lock<std::mutex> lk(lk_fut_execute);
    is_execute_fut_ready = true;
    tasks.on_fut_execute_event = this;
    cv_fut_execute.notify_one();
};

void QueryFuturesEvent::execute()
{
    try_times++;
    p_futexecuteengine->execute();
    finalize();
};

void QueryFuturesEvent::finalize()
{
    des_status = "success";
    end = wall_clock;
    if (next_event)
      next_event->start();
    if (next_parent)
      next_parent->finalize();
};


void QueryFuturesSettlementEvent::start()
{
    beg = wall_clock;
    des_status="active";
    Msg("core", "QueryFuturesSettlement executes");
    std::unique_lock<std::mutex> lk(lk_fut_execute);
    is_execute_fut_ready = true;
    tasks.on_fut_execute_event = this;
    cv_fut_execute.notify_one();    
};


void QueryFuturesSettlementEvent::execute()
{
    try_times++;
    p_futexecuteengine->execute(); 
};


void QueryFuturesSettlementEvent::finalize()
{
    des_status = "success";
    end = wall_clock;
    if (next_event)
      next_event->start();
    if (next_parent)
      next_parent->finalize();
};




void FuturesTradeEvent::start(){
    beg = wall_clock;
    des_status="active";
    try_times++;
    Msg("core", "FuturesTradeEvent starts");
    execute();
};

void FuturesTradeEvent::execute(){
    if (sub_events.size()!=0)
      (*sub_events.begin())->start();
};


void FuturesTradeEvent::finalize()
{
    des_status = "success";
    end = wall_clock;
    if (next_event)
      next_event->start();
    if (next_parent)
      next_parent->finalize();
};

void FuturesComputeEvent::start(){
    beg = wall_clock;
    des_status="active";
    try_times++;
    Msg("core", "FuturesComputeEvent executes");
    std::unique_lock<std::mutex> lk(lk_fut_compute);
    is_compute_fut_ready = true;
    tasks.on_fut_compute_event = this;
    cv_fut_compute.notify_one();
};


void FuturesComputeEvent::execute(){
    p_futcomputeengine->execute();
    need_execute = p_futcomputeengine->need_execute; // see if further execution is needed
    if (need_execute)
    { 
        Msg("fut_compute", futures_order.PrintSummary()) ;
    }
    else{
        Msg("fut_compute", "no further execution");
    }
    finalize();
};


void FuturesComputeEvent::finalize()
{
    des_status = "success";
    end = wall_clock;
    if (next_event)
      next_event->start();
    if (next_parent)
      next_parent->finalize();
};


void FuturesExecuteEvent::start(){
    beg = wall_clock;
    des_status="active";
    try_times++;
    Msg("core", "FuturesExecuteEvent executes");
    std::unique_lock<std::mutex> lk(lk_fut_execute);
    is_execute_fut_ready = true;
    tasks.on_fut_execute_event = this;
    cv_fut_execute.notify_one();
};


void FuturesExecuteEvent::execute(){
    p_futexecuteengine->execute();
    finalize();
};


void FuturesExecuteEvent::finalize()
{
    des_status = "success";
    end = wall_clock;
    if (next_event)
      next_event->start();
    if (next_parent)
      next_parent->finalize();
};


}
