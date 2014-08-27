//
//  element.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/23/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include "ComHeader.h"
#include "StkHeader.h"
#include "StkExecute.h"
#include "global.h"
#include <fstream>
#include "order.h"

using namespace std;

namespace unitrade
{

void StkNormalExecuteEngine::execute( std::shared_ptr<StockOrderBook> p_orderbook){
    Msg("stk_execute", "normal trading strategy initiated");
    
    // use CITICS interface
    STKCITICSOrder stock_order;
    // ********************

    stock_order.Set(make_shared<StockBook>(stock_book), p_orderbook);
    stock_order.Submit();
};


void QueryStockExecuteEngine::execute( ){
    Msg("stk_execute", "query stock account");
    
    // use CITICS interface
    STKCITICSOrder stock_order;
    // ********************
    
    
    stock_order.Set(make_shared<StockBook>(stock_book));
    stock_order.QueryStock();
    stock_book.check(FileLogs);
};

}
