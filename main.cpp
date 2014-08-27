//
//  main.cpp
//  InventoryManagment
//
//  Created by Bin Fan on 7/2/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "StkHeader.h"
#include "FutHeader.h"
#include <thread>
#include <unistd.h>
#include <map>
#include "global.h"
#include <algorithm>
#include <map>
#include "library/bin.h"
#include <mutex>

using namespace std;
using namespace unitrade;

namespace unitrade
{
std::mutex stockbook_mutex;
std::mutex stkexit_mutex;
std::mutex futexit_mutex;
bool stk_exit = false;
bool fut_exit = false;
Logs* FileLogs;
EventsByTime tasks;
}
//This function will be called from a thread


void call_from_thread_monitor(const string name)
{
    thread_status[name] = 1;
    thread_status["ALL"]++;
    std::cerr << "--->>> "<<name<<": launched by core "  << std::endl;
    wall_clock.update();
    
    tasks.ExecuteOnStart();
    
    //while (wall_clock.earlierThan(para.exit_time))
    while (wall_clock.laterThan(para.exit_time))
    {
        stock_book.PrintToFile(FileLogs->stock_monitor);
        stock_book.PrintToBook(FileLogs->stock_book);
        futures_book.PrintToBook(FileLogs->futures_book);
        futures_book.PrintToFile(FileLogs->futures_monitor, para.p_futuresconfig);
        futures_book.updateFuturesBook(stock_book,  para.p_futuresconfig);
        if (condition.is_estimate_settlement)
            futures_book.updateSettlementPrice();
        wall_clock.update();
        tasks.Try();
        tasks.PrintToFile(FileLogs->task_monitor);
        sleep(5);
        condition.Update(wall_clock, para);
    }

    // last works
    tasks.ExecuteOnStop();
    tasks.PrintToFile(FileLogs->task_monitor);   
    
    stkexit_mutex.lock();
    stk_exit = true;
    fut_exit = true;
    stkexit_mutex.unlock();
    
    {
        std::unique_lock<std::mutex> lk_1(lk_stk_compute);
        is_compute_stk_ready = true;
        cv_stk_compute.notify_one(); // wake stk_compute thread up
    }
    {
        std::unique_lock<std::mutex> lk_2(lk_stk_execute);
        is_execute_stk_ready = true;
        cv_stk_execute.notify_one(); // wake stk_execute thread up
    }
    {

        std::unique_lock<std::mutex> lk_3(lk_fut_execute);
        is_execute_fut_ready = true;
        cv_fut_execute.notify_one(); // wake fut_execute thread up
    }
    {
        
        std::unique_lock<std::mutex> lk_4(lk_fut_compute);
        is_compute_fut_ready = true;
        cv_fut_compute.notify_one(); // wake fut_execute thread up
    }
    {
	//MD.stop();
	//TD.stop();
    }


    tasks.PrintToFile(FileLogs->task_monitor);
    stock_book.PrintToFile(FileLogs->stock_monitor);
    stock_book.PrintToBook(FileLogs->stock_book);
    futures_book.PrintToBook(FileLogs->futures_book);
    futures_book.PrintToFile(FileLogs->futures_monitor, para.p_futuresconfig);

    
    thread_status[name] = 2;
    Msg(name, "thread exits");
    thread_status["ALL"]++;
    

}

int main(int argc, char**  argv)
{

    wall_clock.update();
    para.start = wall_clock;
    if (argc >= 2) {
        para.config = argv[1];
    }
    else{
        para.config = "config.ini";
    }
    //getDate(wall_date);
    // parse para
    FileLogs = Logs::getInstance();
    if (para.parse()==-1)
        return -1;
    //if (already_running(para.locker))
    //{
    //    return 1;
    //}
    
    
    FileLogs->OpenIN(para);
    
    // construct stock book
    StockBookFactory(stock_book, para, wall_clock);
    FuturesBookFactory(futures_book, fut_config, wall_clock);

    Msg("core", "reading local stock book");
    stock_book.ReadLocal(para);
    Msg("core", "reading local futures book");
    futures_book.ReadLocal(para);
    
    Msg("core", "reading local event list");
    tasks.ParseEvent(FileLogs->task_list);
    

    FileLogs->OpenOUT(para);
    map<string, std::thread> t;
    std::cerr << "--->>> core: core thread starts\n";

    //Launch a group of threads
    t["stk_quote"]   = std::thread(call_from_thread_stk_quote, "stk_quote");
    usleep(1000);
    t["stk_compute"] = std::thread(call_from_thread_stk_compute, "stk_compute");
    usleep(1000);
    t["stk_execute"] = std::thread(call_from_thread_stk_execute, "stk_execute");
    
    usleep(1000);
    t["fut_quote"]   = std::thread(call_from_thread_fut_quote, "fut_quote");
    
    usleep(1000);
    t["fut_compute"] = std::thread(call_from_thread_fut_compute, "fut_compute");
    
    usleep(1000);
    t["fut_execute"] = std::thread(call_from_thread_fut_execute, "fut_execute");
    
    //usleep(1000);
    //t["inquiry"] = std::thread(call_from_thread, "inquiry");
    
    usleep(1000);
    t["monitor"] = std::thread(call_from_thread_monitor, "monitor");

    
    //Join the threads with the main thread
    t["stk_quote"].join();
    t["stk_compute"].join();
    t["stk_execute"].join();
    //t["inquiry"].join();
    t["fut_quote"].join();
    t["fut_compute"].join();
    t["fut_execute"].join();
    t["monitor"].join();
    
    // clean workspace
    FileLogs->CloseIN();
    FileLogs->CloseOUT();
    ::cerr<<"--->>> core: all threads exit gracefully, goodbye"<<::endl;
    return 0;
}
