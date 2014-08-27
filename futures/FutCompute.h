//
//  FutCompute.h
//  InventoryManagment
//
//  Created by Bin Fan on 7/2/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#ifndef InventoryManagment_FUT_COMPUTE_Header_h
#define InventoryManagment_FUT_COMPUTE_Header_h

#include "ComHeader.h"
#include "global.h"
#include "library/bin.h"
#include "StkHeader.h"
#include "FutHeader.h"
#include <iostream>


using namespace std;

namespace unitrade
{

class TWAPFuturesComputeEngine:public FutComputeEngine{
public:
    TWAPFuturesComputeEngine():FutComputeEngine("TWAP"){
        parse();
    }
    virtual void execute();
    int parse(){
        string spara;
        ifstream ifs;
        ifs.open(bin::FolderAppend(para.p_futuresconfig->FOLDER_TO_STRATEGY, "TWAP.strategy"));
        time_length = 60;
        interval=5;
        if (!ifs.is_open())
        {
            ErrorMsg("cannot open para file for fut-TWAP");
            return -1;
        }

        bin::ParseConfig("TIME_LENGTH", spara, ifs);
        time_length = atof(spara.c_str());
        bin::ParseConfig("INTERVAL", spara, ifs);
        interval = atof(spara.c_str());
        ifs.close();
        return 0;
    }
    int time_length;
    int interval;
};


class FastRollFuturesComputeEngine:public TWAPFuturesComputeEngine{
public:
    FastRollFuturesComputeEngine(){
        name="FastRoll";
        interval = 5;
        time_length = 300;
    }
    virtual void execute();
};

}
#endif
