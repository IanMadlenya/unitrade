#include <iostream>
using namespace std;

class test{
public:
  int i;
  int& getINT1(){return i;}
  int getINT2(){return i;}
  void getADD(){cout<<&i<<endl;}
};

int main(){
  test tt;
  tt.getADD(); // show address of i

  // int& target=tt.getINT2(); 
  // will not compile, convert from rvalue to non-const ref is not permitted
  
  const int& target = tt.getINT2(); // OK
  cout<<&target<<endl;

  return 0;
}
