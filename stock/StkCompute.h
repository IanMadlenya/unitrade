//
//  Header.h
//  InventoryManagment
//
//  Created by Bin Fan on 7/2/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef InventoryManagment_STK_COMPUTE_Header_h
#define InventoryManagment_STK_COMPUTE_Header_h

#include "ComHeader.h"
#include "global.h"
#include "library/bin.h"
#include <iostream>

namespace unitrade
{

class NormalStkComputeEngine:public StkComputeEngine{
public:
    NormalStkComputeEngine():StkComputeEngine("normal"){
    
    }
    virtual void execute(std::shared_ptr<StockOrderBook> p_orderbook);
};


class ManualStkComputeEngine:public StkComputeEngine{
public:
    bool type;
    float extent;
    void ParseTarget();
    void ParseManual();
    std::ifstream source;
    ManualStkComputeEngine():StkComputeEngine("manual"){
        std::ifstream ifs;
        extent = 0;
        ParseManual();
        ParseTarget();
    }
    virtual void execute(std::shared_ptr<StockOrderBook> p_orderbook);
};

}
#endif
