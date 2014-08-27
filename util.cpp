//
//  util.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/22/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include "StkHeader.h"
#include <iostream>
#include <string>
#include "global.h"
#include "library/bin.h"


namespace unitrade
{

int FatalErrorMsg(const std::string msg){
    print_mutex.lock();
    std::cerr<<"!-->>> FATAL ERROR: "<<msg<<std::endl;
    print_mutex.unlock();
    return 1;
}

int ErrorMsg(const std::string msg){
    print_mutex.lock();
    std::cerr<<"!-->>> ERROR: "<<msg<<std::endl;
    print_mutex.unlock();
    return 1;
}

int WarningMsg(const std::string msg){
    print_mutex.lock();
    std::cerr<<"!-->>> Warning: "<<msg<<std::endl;
    print_mutex.unlock();
    return 1;
}


int Msg(const std::string sub, const std::string msg){
    print_mutex.lock();
    std::cerr<<"--->>> "<<sub<<": "<<msg<<std::endl;
    print_mutex.unlock();
    return 0;
}

}
