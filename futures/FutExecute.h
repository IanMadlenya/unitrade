//
//  StkExecute.h
//  InventoryManagment
//
//  Created by Bin Fan on 7/2/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef InventoryManagment_FUT_EXECUTE_Header_h
#define InventoryManagment_FUT_EXECUTE_Header_h

#include "ComHeader.h"
#include <iostream>

namespace unitrade
{

class QueryFuturesExecuteEngine:public FutExecuteEngine{
public:
    QueryFuturesExecuteEngine():FutExecuteEngine("QueryFutures"){}
    virtual void execute();
};


class QueryFuturesSettlementExecuteEngine:public FutExecuteEngine{
public:
    QueryFuturesSettlementExecuteEngine():FutExecuteEngine("QueryFuturesSettlement"){}
    virtual void execute();
};

class NormalFuturesExecuteEngine:public FutExecuteEngine{
public:
    NormalFuturesExecuteEngine():FutExecuteEngine("Normal"){}
    virtual void execute();
};

}
#endif
