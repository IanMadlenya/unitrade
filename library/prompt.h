#ifndef _h_prompt_
#define _h_prompt_

#include <iostream>
#include <vector>

namespace bincon {

class CommandHistory {
 public:
  std::vector<std::string> commands;
  CommandHistory(size_t len = 200) {
    _length = len;
    commands.reserve(len + 1);
  }
  int push_back(const std::string str) {
    commands[current % _length] = str;
    mark = current % _length;
    last++;
    if (last >= _length) last = _length - 1;
    return 0;
  }
  std::string GetNext() {
    if (mark == current)
      return commands[current];
    else if (mark != last)
      return commands[++mark];
    else if (mark == last && mark != current) {
      mark = first;
      return commands[mark];
    }
    return "";
  }
  std::string GetLast() {
    if (mark == first && last == current)
      return commands[mark];
    else if (mark == first && last != current) {
      mark = last;
      return commands[mark];
    }
    else if (mark == current+1)
      return commands[mark];
    else
      return commands[--mark];
  }

 private:
  unsigned int last{};
  unsigned int first{};
  unsigned int current{};
  unsigned int mark{};
  size_t _length;
};
}

#endif
