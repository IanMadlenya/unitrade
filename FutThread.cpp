//
//  StkThread.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/25/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include "global.h"
#include "ComHeader.h"
#include "FutHeader.h"

using namespace std;

//CTPMd MD;
namespace unitrade
{

void call_from_thread_fut_execute(const std::string name){

    thread_status[name] = 1;
    thread_status["ALL"]++;
    std::cerr << "--->>> "<<name<<": launched by core "  << std::endl;
 
    string trader_directory = bin::FolderAppend(para.p_futuresconfig->fut_workspace, "trader")+"/";
       
    //TD.start(trader_directory, para.p_futuresconfig->FRONT_ADDR_TRADE);
    
    std::unique_lock<std::mutex> lk(lk_fut_execute);
    
    while (!is_execute_fut_ready)
    {
        Msg(name, "wait for signal");
        while (!is_execute_fut_ready) {
            cv_fut_execute.wait(lk);
        }
        if (fut_exit)
            break; // if msg says we leave
        else{
            Msg(name, tasks.on_fut_execute_event->name+" execution in process");
            
            // here should be some executation code
            tasks.on_fut_execute_event->execute();
            is_execute_fut_finish = true; // informs stk execute is finished
            is_execute_fut_ready = false; // go back to wait for other execute
            Msg(name, tasks.on_fut_execute_event->name+" execution accomplished");
        }
    }
    //TD.stop();
    thread_status[name] = 2;
    cv.lock();
    Msg(name, "thread exits");
    thread_status["ALL"]++;
    cv.unlock();
}



void call_from_thread_fut_quote(const std::string name){
    //CTPMd MD; 
    thread_status[name] = 1;
    thread_status["ALL"]++;
    std::cerr << "--->>> "<<name<<": launched by core "  << std::endl;
    
    string quote_directory = bin::FolderAppend(para.p_futuresconfig->fut_workspace, "quote")+"/";
       
    //MD.start(quote_directory, para.p_futuresconfig->FRONT_ADDR);
    
    thread_status[name] = 2;
    cv.lock();
    Msg(name, "thread exits");
    thread_status["ALL"]++;
    cv.unlock();

}


void call_from_thread_fut_compute(const string name)
{
    thread_status[name] = 1;
    thread_status["ALL"]++;
    std::cerr << "--->>> "<<name<<": launched by core "  << std::endl;
    
    std::unique_lock<std::mutex> lk(lk_fut_compute);
    while (!is_compute_fut_ready)
    {
        Msg(name, "wait for signal");
        while(!is_compute_fut_ready){
            cv_fut_compute.wait(lk);
        }
        if (fut_exit)
            break; // if msg says we leave
        else{
            Msg(name, tasks.on_fut_compute_event->name+" computation in process");
            
            // here should be some computing code
            tasks.on_fut_compute_event->execute();
            //
            is_compute_fut_finish = true; // informs stk computation is finished
            Msg(name, tasks.on_fut_compute_event->name+" computation accomplished");
            is_compute_fut_ready = false; // go back to wait for other computation
                
        }
    }
    
    thread_status[name] = 2;
    cv.lock();
    Msg(name, "thread exits");
    thread_status["ALL"]++;
    cv.unlock();
    //MD.stop();
}

}
