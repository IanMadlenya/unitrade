//
//  element.cpp
//  StockTrade
//
//  Created by Bin Fan on 7/23/14.
//  Copyright (c) 2014 fanbin. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include "ComHeader.h"
#include "FutCompute.h"
#include "global.h"
#include <fstream>

using namespace std;

namespace unitrade
{

void FastRollFuturesComputeEngine::execute(){
    TWAPFuturesComputeEngine::execute();
}

void TWAPFuturesComputeEngine::execute()
{
    int cur_incre = abs(futures_book.InstrumentList[0]->ExpectPos - futures_book.InstrumentList[0]->Pos);
    int cur_sign = (futures_book.InstrumentList[0]->Pos < futures_book.InstrumentList[0]->ExpectPos? 1:-1);
    int cur_pos = futures_book.InstrumentList[0]->Pos;
    //int cur_target = futures_book.InstrumentList[0]->ExpectPos;

    int next_incre = abs(futures_book.InstrumentList[1]->ExpectPos-futures_book.InstrumentList[1]->Pos);
    int next_sign =  (futures_book.InstrumentList[1]->Pos < futures_book.InstrumentList[1]->ExpectPos? 1:-1);
    int next_pos = futures_book.InstrumentList[1]->Pos;
    //int next_target = futures_book.InstrumentList[1]->ExpectPos;
    
    //int nn_target = futures_book.InstrumentList[2]->ExpectPos;
    int nn_pos    = futures_book.InstrumentList[2]->Pos;
    int nn_sign   = (futures_book.InstrumentList[2]->Pos < futures_book.InstrumentList[2]->ExpectPos? 1:-1);
    int nn_incre = abs(futures_book.InstrumentList[2]->ExpectPos-futures_book.InstrumentList[2]->Pos);
    
    bin::Timer StartTime = wall_clock;
    bin::Timer EndTime = StartTime;
    EndTime.add(time_length);
    
    Msg("fut_compute",name+" slot "+StartTime.printShortTime()+"-->"+EndTime.printShortTime());
    
    if (cur_incre==0 && next_incre==0 && nn_incre==0)
        return; // no new order is needed
    
    shared_ptr<FuturesOrder> order1, order2, order3, order4, order5, order6;
        
    vector<int> slot_num_cur, slot_num_next, slot_num_nn;
    vector<string> slot_time_str;

    bin::Timer slot_time = StartTime;
    
    if (wall_clock.laterThan(slot_time))
        slot_time=wall_clock;
    
    if (wall_clock.laterThan(EndTime))
    {
            cerr<<"--->>> fut_compute ERROR: later than end time, something is wrong"<<endl;
            return ;
    }
    
    // construct time slots
    while (slot_time.earlierThan(EndTime))
    {
        if ( slot_time.isBetween(para.p_futuresconfig->NoonBreakStart,para.p_futuresconfig->NoonBreakEnd) )
        {
                slot_time = para.p_futuresconfig->NoonBreakEnd;
                slot_time.add(60);
                continue;
        }
        if (cur_incre>0)
        {
                slot_num_cur.push_back(1);
                cur_incre--;
        }
        if (next_incre>0)
        {
                slot_num_next.push_back(1);
                next_incre--;
        }
        if (nn_incre>0)
        {
                slot_num_nn.push_back(1);
                nn_incre--;
        }
            //cout<<nn_incre<<endl;
            //cout<<slot_time.printTime()<<endl;
        slot_time_str.push_back(slot_time.printShortTime());
        slot_time.add(interval);
    }
    
    // allocate order size into time slots
    unsigned long i_incre = slot_num_cur.size()-1;
    while (cur_incre > 0)
    {
            
            slot_num_cur[i_incre] += 1;
            if (i_incre!=0)
                i_incre--;
            else
                i_incre = slot_num_cur.size()-1;
            cur_incre--;
    }
        
        
    i_incre = slot_num_next.size()-1;
    while (next_incre > 0)
    {
            slot_num_next[i_incre] += 1;
            if (i_incre!=0)
                i_incre--;
            else
                i_incre = slot_num_next.size()-1;
            next_incre--;
            
    }
        
        
    i_incre = slot_num_nn.size()-1;
    while (nn_incre > 0)
    {
            slot_num_nn[i_incre] += 1;
            if (i_incre!=0)
                i_incre--;
            else
                i_incre = slot_num_nn.size()-1;
            nn_incre--;
    }
        
    string offset, dir;
    double price=0.0;
    int amount=0;
        
    i_incre = 0;
    string tt;
    while ( i_incre<slot_num_next.size() ||  i_incre<slot_num_cur.size() || i_incre<slot_num_nn.size() )
    {
        tt = slot_time_str[i_incre];
        order1 = NULL;
        order2 = NULL;
        order3 = NULL;
        order4 = NULL;
        order5 = NULL;
        order6 = NULL;
        dir = offset = "";
        if (i_incre<slot_num_cur.size() )
        {
                amount = slot_num_cur[i_incre];
                if (cur_sign>0)
                {
                    dir = "buy";
                    price = futures_book.InstrumentList[0]->bid1 - 0.2;
                }
                else
                {
                    price = futures_book.InstrumentList[0]->ask1 + 0.2;
                    dir = "sell";
                }
                
                if (cur_pos*cur_sign<0)
                    offset="close";
                else
                    offset="open";
                if (offset=="close" && abs(cur_pos)<amount )
                {
                    order1 = make_shared<FuturesOrder>(futures_book.InstrumentList[0]->_code,
                                            "market",
                                            offset,
                                            dir,
                                            abs(cur_pos),
                                            price);
                    order3 = make_shared<FuturesOrder>(futures_book.InstrumentList[0]->_code,
                                            "market",
                                            "open",
                                            dir,
                                            amount-abs(cur_pos),
                                            price);
                    futures_order.cnt_ActiveOrders += 2;
                    order1->schedule.set(tt);
                    order3->schedule.set(tt);
                    tt = order1->schedule.printShortTime();
                    order1->InitTime=tt;
                    order3->InitTime=tt;
                }
                else
                {
                    order1 = make_shared<FuturesOrder>(futures_book.InstrumentList[0]->_code,
                                            "market",
                                            offset,
                                            dir,
                                            amount,
                                            price);
                    futures_order.cnt_ActiveOrders += 1;

                    order1->schedule.set(tt);
                    tt = order1->schedule.printShortTime();
                    order1->InitTime=tt;
                }
                cur_pos += cur_sign * amount;
                
                
            }
            
            // process next month's order
            if (i_incre<slot_num_next.size() )
            {
                amount=0;
                price=0.0;
                dir = offset = "";
                amount = slot_num_next[i_incre];
                
                if (next_sign>0)
                {
                    dir = "buy";
                    price = futures_book.InstrumentList[1]->bid1 - 0.2;
                }
                else
                {
                    price = futures_book.InstrumentList[1]->ask1 + 0.2;
                    dir = "sell";
                }
                
                if (next_pos*next_sign<0)
                    offset="close";
                else
                    offset="open";
                if (offset=="close" && abs(next_pos)<amount )
                {
                    order2 = make_shared<FuturesOrder>(futures_book.InstrumentList[1]->_code,
                                            "market",
                                            offset,
                                            dir,
                                            abs(next_pos),
                                            price);
                    order4 = make_shared<FuturesOrder>(futures_book.InstrumentList[1]->_code,
                                            "market",
                                            offset,
                                            dir,
                                            amount-abs(next_pos),
                                            price);
                    futures_order.cnt_ActiveOrders += 2;
                    order2->schedule.set(tt);
                    order4->schedule.set(tt);
                    tt = order2->schedule.printShortTime();
                    order2->InitTime = tt;
                    order4->InitTime = tt;
                }
                else
                {
                    order2 = make_shared<FuturesOrder>(futures_book.InstrumentList[1]->_code,
                                            "market",
                                            offset,
                                            dir,
                                            amount,
                                            price);
                    futures_order.cnt_ActiveOrders += 1;
                    order2->schedule.set(tt);
                    tt = order2->schedule.printShortTime();
                    order2->InitTime = tt;
                }
                next_pos += next_sign*amount;
                
            }
            //cout<<slot_num_nn.size()<<endl;
            // process next-next month contracts
            if (i_incre<slot_num_nn.size() )
            {
                amount=0;
                price=0.0;
                dir = offset = "";
                amount = slot_num_nn[i_incre];
                
                if (nn_sign>0)
                {
                    dir = "buy";
                    price = futures_book.InstrumentList[2]->bid1 + 0.2;
                }
                else
                {
                    price = futures_book.InstrumentList[2]->ask1 - 0.2;
                    dir = "sell";
                }
                
                if (nn_pos*nn_sign<0)
                    offset="close";
                else
                    offset="open";
                if (offset=="close" && abs(nn_pos)<amount )
                {
                    order5 = make_shared<FuturesOrder>(futures_book.InstrumentList[2]->_code,
                                            "limit",
                                            offset,
                                            dir,
                                            abs(nn_pos),
                                            price);
                    order6 = make_shared<FuturesOrder>(futures_book.InstrumentList[2]->_code,
                                            "limit",
                                            offset,
                                            dir,
                                            amount-abs(nn_pos),
                                            price);
                    futures_order.cnt_ActiveOrders += 2;
                    order5->schedule.set(tt);
                    order6->schedule.set(tt);
                    tt = order5->schedule.printShortTime();
                    order5->InitTime = tt;
                    order6->InitTime = tt;
                }
                else
                {
                    order5 = make_shared<FuturesOrder>(futures_book.InstrumentList[2]->_code,
                                            "limit",
                                            offset,
                                            dir,
                                            amount,
                                            price);
                    futures_order.cnt_ActiveOrders += 1;
                    order5->schedule.set(tt);
                    tt = order5->schedule.printShortTime();
                    order5->InitTime = tt;
                }
                nn_pos += nn_sign * amount;
                
            }
            
            
            //  cerr<<cur_incre<<"\t"<<next_incre<<"\t"<<cur_pos<<"\t"<<next_pos<<"\t"<<amount<<endl;
            //  sleep(1);
            if (order5)
                futures_order.insertOrder(order5);
            if (order6)
                futures_order.insertOrder(order6);
            if (order1)
                futures_order.insertOrder(order1);
            if (order3)
                futures_order.insertOrder(order3);
            if (order2)
                futures_order.insertOrder(order2);
            if (order4)
                futures_order.insertOrder(order4);
            i_incre++;
        }
    return;
}

}
