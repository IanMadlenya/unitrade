//
//  main.cpp
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "HDFengine.h"
#include "stsdb.h"
#include "bin.h"
#include "csv.h"
#include <thread>
#include <tbb/tbb.h>
#include <tbb/parallel_for.h>

using namespace std;
using namespace stsdb;

int main(int argc, const char* argv[]) {
  // construct timeseries
  try {
    TSPtr<StockDailyRecord> ts;
    vector<TSPtr<StockDailyRecord>> list_ts;
    list_ts.reserve(3000);
    std::shared_ptr<HDFrecord<StockDailyRecord>> p_HDF =
        std::make_shared<HDFrecord<StockDailyRecord>>(argv[1]);
    if (p_HDF->Init()) {
      std::cerr << "ERROR in open output file" << std::endl;
      return -1;
    }

    // timer starts
    bin::StopWatch stop_watch;
    stop_watch.start();
    // typedef std::chrono::high_resolution_clock Clock;
    // auto epoch = Clock::now();
    // ************

    p_HDF->Read(&list_ts);

    stop_watch.stop();
    cerr << "\n--->>> stsdb: costs=";
    stop_watch.print();

    // auto duration = Clock::now() - epoch;
    // std::cerr << "\n--->>> STSDB: cost "
    //          <<
    //          std::chrono::duration_cast<std::chrono::milliseconds>(duration)
    //                 .count() << " ms" << std::endl;
    // ************

  } catch (const TimeseriesException& e) {
    std::cerr << e.what() << endl;
    return -1;
  }

  
 
  return 0;
}

