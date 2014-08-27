#include <string>
#include <iostream>
#include <stdint.h>
#include <chrono>
#include <ctime>
#include "security.h"
#include "bin.h"
#include "license.h"

using namespace std;
typedef std::chrono::system_clock Clock;
int main(int argc, char *argv[]) {
  // bin::Date mydate;
  if (argc != 3) {
    cerr << "Usage: codegen <license> <new expire day>" << endl;
    return -1;
  }
  binsec::License license;
  bin::Date mydate;
  if (bin::isFileExist(argv[1])) {
    license.Load(argv[1]);
    cout << "--->>> License: license says " << license.time_to_expire << " days left"
         << endl;
  }
  else
  {
    cout<<"--->>> License: cannot find license file="<<argv[1]<<endl; 
    license.created = mydate;
  }

  license.last_modified = mydate;
  license.time_to_expire = atoi(argv[2]);
  cout << "--->>> License: "
       << "now " << license.time_to_expire << " days left" << endl;

  license.Save(argv[1]);

  return 0;
}
