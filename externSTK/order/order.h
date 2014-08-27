//
//  order.h
//  StockTrade
//
//  Created by Bin Fan on 8/2/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#ifndef __StockTrade__order__
#define __StockTrade__order__

#include "StkHeader.h"

namespace unitrade
{

class STKCITICSOrder{
public:
    void Set(std::shared_ptr<StockBook> sb, std::shared_ptr<StockOrderBook> so=nullptr){
        stock_book = sb;
        stock_order = so;
    }
    std::shared_ptr<StockBook> stock_book;
    std::shared_ptr<StockOrderBook> stock_order;
    int QueryStock();
    int Submit();
    int QueryOrder();
    int Cancel();
    int ReSubmit();
};

}
#endif /* defined(__StockTrade__order__) */
