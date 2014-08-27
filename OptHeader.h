#ifndef _h_option_header_
#define _h_option_header_

#include <memory>

namespace unitrade {


enum OptionType {
  European,
  American,
  Asia
};

enum OptionStyle {
  Barrier,
  Binary,
  Exotic,
  Vanilla
};

class Stock;
class Futures;

class Option : public security {
 public:
  OptionStyle option_style;
  OptionType option_type;
  float stike;
  std::shared_ptr<security> underlying;
}


}

#endif
