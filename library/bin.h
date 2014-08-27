#ifndef _bin_h_
#define _bin_h_ 1

#include <string>
#include <ctime>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <sstream>
#include <chrono>
#include "csv.h"
#include "progress.h"
#include "g2u.h"
#include "int2string.h"
#include "parse.h"
#include "currency.h"
#include "unixsystem.h"
#include "bintime.h"
#include "stream_conversation.h"
#include "optionparser.h"
#include "parseconfig.h"
#include "parsecommand.h"
#include "prompt.h"

//-------------Convert from numerical to string-----



namespace bin
{
//*************** System Optimization Params ****************
#include "system.inc"
//***********************************************************






std::string ParseCommand(char *m, const char *h);
std::string ParseCommand(std::string message, const char *h);
std::vector<std::string> ParseCommandList(char *m);
std::vector<std::string> ParseCommandList(std::string &s);






//------------Parse and Manipulate Config file Functions--------
int ReplaceValue(std::string file, std::string key, std::string value);




//----------------------- Security Functions--------------------------


//--------------------------------------------------------------------


//-------------classes-----------------
class StopWatch {
 private:
  std::chrono::high_resolution_clock::time_point
      epoch, ending;
  std::chrono::high_resolution_clock::duration elapse;

 public:
  void start() {
    typedef std::chrono::high_resolution_clock Clock;
    epoch = Clock::now();
  }
  void stop() {
    typedef std::chrono::high_resolution_clock Clock;
    ending = Clock::now();
    elapse = ending - epoch;
  }
  void print() {
    std::cerr << std::chrono::duration_cast<std::chrono::milliseconds>(elapse)
                     .count() << " ms" << std::endl;
  }
  void reset() {
    epoch = {};
    ending = {};
    elapse = {};
  }
};


}
#endif
