
#ifndef _h_action_
#define _h_action_

#include <string>
#include <iostream>
#include "bin.h"

namespace unitrade{
extern binsec::License license;
}

namespace bincon {

inline int ActionOnCommand(bin::CommandPacket &command) { 
  
  if (command.code=="SetLicense")
  {
    using unitrade::license;
    int licenseday = atoi(command.argumentlist["<day>"].c_str());

    if (command.argumentlist["<day>"][0]=='+'||command.argumentlist["<day>"][1]=='-')
       unitrade::license.time_to_expire += licenseday;
    else
       unitrade::license.time_to_expire = licenseday;
    bin::Date mydate;
    unitrade::license.last_modified = mydate;
    unitrade::license.Save();
    command.answer = "status=OK&"+unitrade::license.CompileLicense();
  }
  else if (command.code=="ShowLicense")
  {
    using unitrade::license;
    command.answer = "status=OK&"+unitrade::license.CompileLicense();
  }
  
  
  return 0;
};


}
#endif
