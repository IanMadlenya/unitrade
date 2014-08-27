#ifndef _bin_h_progress_
#define _bin_h_progress_ 1

#include <iostream>


namespace bin
{
inline void DisplayProgress(float progress) {
  int barWidth = 50;
  std::cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %\r";
  std::cout.flush();

}

}
#endif
