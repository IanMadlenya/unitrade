//
//  FutExecute.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/23/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include "ComHeader.h"
#include "FutHeader.h"
#include "FutExecute.h"
#include "global.h"
#include <fstream>

using namespace std;


namespace unitrade
{

void QueryFuturesExecuteEngine::execute(){
    Msg("fut_execute", "query futures initiated");
    //TraderSpi->ReqQryTradingAccount();
    sleep(1);
    for (unsigned int i=0; i<para.p_futuresconfig->INSTRUMENT_NAME_LIST.size() ; ++i)
    {
        //TraderSpi->ReqQryInvestorPosition(const_cast<char*>( para.p_futuresconfig->INSTRUMENT_NAME_LIST[i].c_str()) );
        sleep(1);
    }
};



void QueryFuturesSettlementExecuteEngine::execute(){
    Msg("fut_execute", "query futures settlement");

};



void NormalFuturesExecuteEngine::execute(){
    Msg("fut_execute", "normal trading strategy initiated");
    
};

}
