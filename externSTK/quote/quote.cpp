//
//  quote.cpp
//  StockTrade
//
//  Created by Bin Fan on 8/1/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include "quote.h"
#include "ReadDBF.h"

namespace unitrade
{

int STKDBFQuote::update(){
    parseQuoteByDBF(stockbook, dbf_shanghai, dbf_shenzhen);
    return 0;
};

}
