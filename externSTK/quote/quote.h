//
//  quote.h
//  StockTrade
//
//  Created by Bin Fan on 8/1/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#ifndef __StockTrade__quote__
#define __StockTrade__quote__

#include "StkHeader.h"

namespace unitrade
{

class STKDBFQuote{
public:
    void Set(std::shared_ptr<StockBook> s, Para& p){
        stockbook = s;
        dbf_shanghai = p.p_stockconfig->shanghai_quote;
        dbf_shenzhen = p.p_stockconfig->shenzhen_quote;
    }
    std::string dbf_shanghai, dbf_shenzhen;
    std::shared_ptr<StockBook> stockbook;
    int update();
};

}
#endif /* defined(__StockTrade__quote__) */
