//
//  main.cpp
//  stsdb
//
//  Created by Bin Fan on 8/4/14.
//  Copyright (c) 2014 Bin Fan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "stsdb.h"
#include "HDFengine.h"
#include "bin.h"
#include "csv.h"
#include <thread>
#include <tbb/tbb.h>
#include <tbb/parallel_for.h>


using namespace std;
using namespace stsdb;

int main(int argc, const char* argv[]) {
  cout<<std::thread::hardware_concurrency()<<endl;
  std::ifstream file;
  if (argc != 3)
  {
    std::cerr<<" Usage: ./import [path-to-data-list] [path-to-hdf]"<<std::endl;
    return 1;
  }
  file.open(argv[1]);
  if (!file.is_open()) {
    std::cerr << "cannot open file=" << argv[1] << std::endl;
    return -1;
  }

  // construct timeseries
  try {
    stsdb::CSVRow listrow;
    file >> listrow;  // first row as header

    string source;
    std::shared_ptr<HDFrecord<StockDailyRecord>> p_HDF;
    int progress = 0;
    TSPtr<StockDailyRecord> ts;
    vector<TSPtr<StockDailyRecord>> list_ts;
    list_ts.reserve(3000);
    p_HDF = std::make_shared<HDFrecord<StockDailyRecord>>(argv[2]);
    if (p_HDF->Init()) {
      std::cerr << "ERROR in open output file" << std::endl;
      return -1;
    }

    bool is_first_line = true;
    size_t max_size{};  // this is used for vector.reserve
    while (file >> listrow) {
      ts = make_shared<Timeseries<StockDailyRecord>>(
          listrow[0],  // code
          listrow[5],  // data Chinese name
          "quote",     // dataset name
          listrow[1]   // gics
          );
      list_ts.push_back(ts);
    }

    ifstream ifs;
    string date;
    float open, close, high, low, prev, vol;
    double amt;

    vector<StockDailyRecord> data;
    for (auto  it:list_ts){  
      source = bin::FolderAppend(bin::DirName(argv[1]), it->getCODE() + ".csv");
      ifs.open(source.c_str());
      if (!ifs.is_open()) {
        std::cerr << source << " cannot open" << endl;

        continue;
      }
      if (is_first_line) {
        max_size = bin::CountLine(ifs) + 3;
        is_first_line = false;
      }
 
      progress++;
      bin::DisplayProgress(progress*1.0/list_ts.size());
      data.clear();
  
      data.reserve(max_size);
      io::CSVReader<8, io::trim_chars<' '>, io::no_quote_escape<','> > in(source);
      in.read_header(io::ignore_extra_column, "Date", "Open", "Close", "High",
                     "Low", "Pre", "Volume", "Amount");
      while (in.read_row(date, open, close, high, low, prev, vol, amt)) {
          data.emplace_back(StockDailyRecord{parseTimeToStamp(date, "%Y-%m-%d"),
                                           open, close, high, low, prev, vol,
                                           amt});
      }
      //cout<<data.size()<<endl;
      it->appendRecord(data);
      //cout<<it->getNRecords()<<endl;
      ifs.close(); 
    }

    // timer starts
    bin::StopWatch stop_watch;
    stop_watch.start();
    //typedef std::chrono::high_resolution_clock Clock;
    //auto epoch = Clock::now();
    // ************

    for (auto i : list_ts) {
      p_HDF->Write(i);
    }
    //p_HDF->Read<StockDailyRecord>();

    stop_watch.stop();
    cerr<<"\n--->>> stsdb: costs=";
    stop_watch.print();
    //auto duration = Clock::now() - epoch;
    //std::cerr << "\n--->>> STSDB: cost "
    //          << std::chrono::duration_cast<std::chrono::milliseconds>(duration)
    //                 .count() << " ms" << std::endl;
    // ************

  } catch (const TimeseriesException& e) {
    std::cerr << e.what() << endl;
    return -1;
  }
  return 0;
}

