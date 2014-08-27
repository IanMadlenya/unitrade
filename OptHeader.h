#ifndef  _h_option_header_
#define  _h_option_header_

#include <memory>

namespace unitrade
{

class Stock;
class Futures;

class Option : public security
{
  std::shared_ptr<security> underlying;

}


}

#endif
