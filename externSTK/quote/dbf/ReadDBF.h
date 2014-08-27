//
//  ReadDBF.h
//  StockTrade
//
//  Created by Bin Fan on 8/1/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#ifndef StockTrade_ReadDBF_h
#define StockTrade_ReadDBF_h
#include "StkHeader.h"

namespace unitrade
{

unsigned int UnpackBigEndian4(unsigned char *p);

unsigned int UnpackBigEndian2(unsigned char *p);

int parseQuoteByDBF(std::shared_ptr<StockBook> stock_book, std::string, std::string);

}
#endif
