//
//  StkThread.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/25/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include "global.h"
#include "ComHeader.h"
#include "StkHeader.h"
#include <unistd.h>
#include "order.h"

using namespace std;


namespace unitrade
{

void call_from_thread_stk_quote(const string name)
{
    thread_status[name] = 1;
    thread_status["ALL"]++;
    std::cerr << "--->>> "<<name<<": launched by core "  << std::endl;
    wall_clock.update();
    STKDBFQuote stock_quote;
    stock_quote.Set(make_shared<StockBook>(stock_book), para);

    while(!stk_exit)
    {
        stockbook_mutex.lock(); // or, to be exception-safe, use std::lock_guard
        stock_quote.update();
        stock_book.compile();
        stockbook_mutex.unlock();
        sleep(5);
    }
    thread_status[name] = 2;
    cv.lock();
    Msg(name, "thread exits");
    thread_status["ALL"]++;
    cv.unlock();

}

void call_from_thread_stk_compute(const string name)
{
    thread_status[name] = 1;
    thread_status["ALL"]++;
    std::cerr << "--->>> "<<name<<": launched by core "  << std::endl;
    
    std::unique_lock<std::mutex> lk(lk_stk_compute);
    while (!is_compute_stk_ready)
    {
        Msg(name, "wait for signal");
        while(!is_compute_stk_ready){
            cv_stk_compute.wait(lk);
        }
        if (stk_exit)
            break; // if msg says we leave
        else{
            Msg(name, tasks.on_stk_compute_event->name+" in process");

            // here should be some computing code
            tasks.on_stk_compute_event->execute();
            //
            
            is_compute_stk_finish = true; // informs stk computation is finished
            Msg(name, tasks.on_stk_compute_event->name+" accomplished");
            is_compute_stk_ready = false; // go back to wait for other computation            
        }
    }
    
    thread_status[name] = 2;
    cv.lock();
    Msg(name, "thread exits");
    thread_status["ALL"]++;
    cv.unlock();
}


void call_from_thread_stk_execute(const string name)
{
    thread_status[name] = 1;
    thread_status["ALL"]++;
    std::cerr << "--->>> "<<name<<": launched by core "  << std::endl;
    while (!is_execute_stk_ready)
    {
        std::unique_lock<std::mutex> lk(lk_stk_execute);
        Msg(name, "wait for signal");
        while(!is_execute_stk_ready){
            cv_stk_execute.wait(lk);
        }
        if (stk_exit)
            break; // if msg says we leave, ready to exit
        else{
            Msg(name, tasks.on_stk_execute_event->name+" in process");

            // here should be some executation code
            tasks.on_stk_execute_event->execute();
            is_execute_stk_finish = true; // informs stk execute is finished
            is_execute_stk_ready = false; // go back to wait for other execute
            Msg(name, tasks.on_stk_execute_event->name+" accomplished");
        }
    }
    thread_status[name] = 2;
    cv.lock();
    Msg(name, "thread exits");
    thread_status["ALL"]++;
    cv.unlock();
}

}
