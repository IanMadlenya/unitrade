#include "UtilHeader.h"

using namespace std;

int main(){
  unitrade::Calendar cal;
  cal.Load("/var/hq/dbcn/cal_cn.txt");

  for (auto i:cal.trading_days)
    cout<<i<<endl;
  cout<<"Trading Day?"<<(cal.isTradingDay?"Yes":"No")<<endl;
  return 0;
}
