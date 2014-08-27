#ifndef _bin_currency_
#define _bin_currency_ 1


#include <iostream>
#include <stack>
#include <algorithm>
using namespace std;

namespace bin
{

inline std::string PrintCurrency(std::string currency) {
  std::string new_string;
  std::string::iterator it;
  std::stack<char> T;
  const unsigned long n = currency.length();
  for (unsigned int i = 0; i < n; i++) T.push(currency[i]);
  int j = 1;
  for (unsigned int i = 0; !T.empty(); ++i, T.pop()) {
    new_string += T.top();
    if (j == 3) {
      j = 0;
      new_string += ',';
    }
    j++;
  }
  std::reverse(new_string.begin(), new_string.end());
  return new_string;
}

}
#endif
/*
int main(int argc, char *argv[]) {
        string newid= PrintCurrency("10002345323");
        cout<<newid<<endl;
}
*/
