//
//  StkExecute.h
//  InventoryManagment
//
//  Created by Bin Fan on 7/2/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef InventoryManagment_STK_EXECUTE_Header_h
#define InventoryManagment_STK_EXECUTE_Header_h

#include "ComHeader.h"
#include <iostream>

namespace unitrade
{

class StkNormalExecuteEngine:public StkExecuteEngine{
public:
    StkNormalExecuteEngine():StkExecuteEngine("normal"){}
    virtual void execute( std::shared_ptr<StockOrderBook> p_orderbook);
};

class QueryStockExecuteEngine:public StkExecuteEngine{
public:
    QueryStockExecuteEngine():StkExecuteEngine("QueryStock"){}
    virtual void execute();
};

}
#endif
