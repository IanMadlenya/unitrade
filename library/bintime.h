#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include "bin.h"
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
using namespace std;

namespace bin {

inline std::string printFileTimeStamp(const string path) {
  struct stat attr;
  stat(path.c_str(), &attr);
  char date[30];
  strftime(date, 30, "%H:%M:%S", localtime(&(attr.st_mtime)));
  return string(date);
}

inline std::string printTime(time_t TIME = 0, bool pad = true) {
  std::string final;
  std::stringstream ss;
  if (TIME == 0)
    TIME = time(NULL);  // if no argument is fed, then use the current time
  struct tm *tinfo = localtime(&TIME);
  ss << tinfo->tm_hour;
  final = ss.str();
  ss.str("");
  ss << tinfo->tm_min;
  if (pad && tinfo->tm_min < 10)
    final = final + ":0" + ss.str();
  else
    final = final + ":" + ss.str();

  ss.str("");
  ss << tinfo->tm_sec;
  if (pad && tinfo->tm_sec < 10)
    final = final + ":0" + ss.str();
  else
    final = final + ":" + ss.str();
  ss.str("");
  return final;
}

inline std::string printDate(int year=0, int month=0, int day=0, int format = 0,
                             bool pad = true) {
  /*
   format = 0 : 20130201  (Default)
   format = 1 : 2012-02-01
   format = 2 : 02/01/2012 (U.S.A.)
   format = 3 : 01/02/2012 (UK)
   */
  std::string final;
  std::stringstream ss;
  time_t TIME = time(NULL);
  struct tm *tinfo = localtime(&TIME);

  if (year==0&&month==0&&day==0)
  {
    year = tinfo->tm_year + 1900;
    month = tinfo->tm_mon + 1;
    day = tinfo->tm_mday;
  }
  if (format == 0) {
    ss << year;
    final = ss.str();
    ss.str("");
    ss << month;
    if (month < 10)
      final = final + "0" + ss.str();
    else
      final = final + ss.str();
    ss.str("");
    ss << day;
    if (day < 10)
      final = final + "0" + ss.str();
    else
      final = final + ss.str();
    ss.str("");
  } else if (format == 1) {
    ss << year;
    final = ss.str();
    ss.str("");
    ss << month;
    if (pad && month < 10)
      final = final + "-0" + ss.str();
    else
      final = final + "-" + ss.str();
    ss.str("");
    ss << day;
    if (pad && day < 10)
      final = final + "-0" + ss.str();
    else
      final = final + "-" + ss.str();
    ss.str("");
  } else if (format == 2) {
    ss << month;
    if (pad && month < 10)
      final = "0" + ss.str();
    else
      final = ss.str();
    ss.str("");
    ss << day;
    if (pad && day < 10)
      final = "0" + final + "/" + ss.str();
    else
      final = final + "/" + ss.str();
    ss.str("");
    ss << year;
    final = final + "/" + ss.str();
    ss.str("");
  } else if (format == 3) {
    ss << day;
    if (pad && day < 10)
      final = "0" + ss.str();
    else
      final = ss.str();
    ss.str("");
    ss << month;
    if (pad && month < 10)
      final = final + "/0" + ss.str();
    else
      final = final + "/" + ss.str();
    ss.str("");
    ss << year;
    final = final + "/" + ss.str();
    ss.str("");
  }
  return final;
}

inline int ParseDate(std::string TT, int &y, int &m, int &d, int format = 0) {
  /*
   format = 0 : 20130201  (Default)
   format = 1 : 2012-02-01
   format = 2 : 02/01/2012 (U.S.A.)
   format = 3 : 01/02/2012 (UK)
   */
  string year, month, day;
  if (format == 0) {
    year = TT.substr(0, 4);
    month = TT.substr(4, 2);
    day = TT.substr(6, 2);
    y = atoi(year.c_str());
    m = atoi(month.c_str());
    d = atoi(day.c_str());
  } else if (format == 1) {
    year = TT.substr(0, 4);
    month = TT.substr(5, 2);
    day = TT.substr(8, 2);
    y = atoi(year.c_str());
    m = atoi(month.c_str());
    d = atoi(day.c_str());
  } else if (format == 2) {
    year = TT.substr(6, 4);
    month = TT.substr(0, 2);
    day = TT.substr(3, 2);
    y = atoi(year.c_str());
    m = atoi(month.c_str());
    d = atoi(day.c_str());
  } else if (format == 3) {
    year = TT.substr(6, 4);
    month = TT.substr(3, 2);
    day = TT.substr(0, 2);
    y = atoi(year.c_str());
    m = atoi(month.c_str());
    d = atoi(day.c_str());
  }
  return 0;
}

inline int ParseTime(std::string TT, int &h, int &m, int &s) {
  /*
      return 0: everything OK
      return 1: malfunction
      return -1: functioning good, but time is obviously mistaken, e.g. 27:13:13
  */

  vector<string> tokens;
  string item;
  string sh, sm, ss;
  string::size_type position;
  position = TT.find(":");
  if (position != TT.npos) {
    stringstream iss(TT);
    while (std::getline(iss, item, ':')) {
      tokens.push_back(item);
    }
    ss = "0";
    if (tokens.size() < 2 || tokens.size() > 3)
      return 1;
    else if (tokens.size() == 3)
      ss = tokens[2];
    sh = tokens[0];
    sm = tokens[1];
  } else if (TT.size() == 6) {
    sh = TT.substr(0, 2);
    sm = TT.substr(2, 2);
    ss = TT.substr(4, 2);
  } else if (TT.size() == 4) {
    sh = TT.substr(0, 2);
    sm = TT.substr(2, 2);
  } else {
    return 1;
  }
  s = atoi(ss.c_str());
  h = atoi(sh.c_str());
  m = atoi(sm.c_str());
  if (h <= 24 && h >= 0 && m >= 0 && m < 60 && s >= 0 && s < 60)
    return 0;
  else
    return -1;
}

inline long ParseTimeToSecond(const std::string &TT) {
  int h, m, s;
  h = m = s = 0;
  ParseTime(TT, h, m, s);
  return (long)(h * 3600 + m * 60 + s);
}

class Timer {
 public:
  int hour;
  int min;
  int sec;
  int millisec;
  long core;
  Timer() {
    hour = 0;
    min = 0;
    sec = 0;
    millisec = 0;
  };
  Timer(int h, int m, int s, int ms = 0) {
    hour = h;
    min = m;
    sec = s;
    millisec = ms;
  };
  Timer(const class Timer &a) {
    hour = a.hour;
    min = a.min;
    sec = a.sec;
    millisec = a.millisec;
  };
  Timer &operator=(const Timer &a) {
    hour = a.hour;
    min = a.min;
    sec = a.sec;
    millisec = a.millisec;
    return *this;
  };
  int validate() {
    min += int(sec / 60);
    sec = sec % 60;
    hour += int(min / 60);
    min = min % 60;
    return 0;
  }
  Timer &operator+(const int s) {
    sec += s;
    validate();
    return *this;
  }
  int set(int h, int m, int s, int ms = 0) {
    hour = h;
    min = m;
    sec = s;
    millisec = ms;
    return 0;
  }
  int update() {
    struct timeval now;
    gettimeofday(&now, NULL);
    time_t rawtime = now.tv_sec;
    struct tm *lt = localtime(&rawtime);
    core = now.tv_sec;
    millisec = now.tv_usec / 1000;
    hour = lt->tm_hour;
    min = lt->tm_min;
    sec = lt->tm_sec;
    return 0;
  };

  int update(int h, int m, int s, int ms) {
    hour = h;
    min = m;
    sec = s;
    millisec = ms;
    return 0;
  };

  bool earlierThan(class Timer &point) {
    return ((point.hour - hour) * 3600 + (point.min - min) * 60 +
                        (point.sec - sec) +
                        0.001 * (point.millisec - millisec) >=
                    0
                ? 1
                : 0);
  };

  bool laterThan(class Timer &point) {
    return ((point.hour - hour) * 3600 + (point.min - min) * 60 +
                        (point.sec - sec) +
                        0.001 * (point.millisec - millisec) <=
                    0
                ? 1
                : 0);
  };

  bool earlierThan(std::string &point) {
    int ph, pm, ps;
    ParseTime(point, ph, pm, ps);
    return ((ph - hour) * 3600 + (pm - min) * 60 + (ps - sec) >= 0 ? 1 : 0);
  };

  bool laterThan(std::string &point) {
    int ph, pm, ps;
    ParseTime(point, ph, pm, ps);
    return ((ph - hour) * 3600 + (pm - min) * 60 + (ps - sec) <= 0 ? 1 : 0);
  };

  bool isBetween(class Timer &start, class Timer &end) {
    return (((start.hour - hour) * 3600 + (start.min - min) * 60 +
                 (start.sec - sec) + 0.001 * (start.millisec - millisec) <=
             0) && ((end.hour - hour) * 3600 + (end.min - min) * 60 +
                        (end.sec - sec) + 0.001 * (end.millisec - millisec) >=
                    0)
                ? 1
                : 0);
  };

  int Interval(class Timer &start) {
    if (laterThan(start)) {
      return (hour - start.hour) * 3600 + (min - start.min) * 60 +
             (sec - start.sec) + 0.001 * (millisec - start.millisec);
    } else {
      return (start.hour - hour) * 3600 + (start.min - min) * 60 +
             (start.sec - sec) + 0.001 * (start.millisec - millisec);
    }
  };

  int toSec() { return hour * 3600 + min * 60 + sec; };

  int add(int s) {
    int ss = toSec();
    ss += s;

    hour = int(ss / 3600);
    ss -= hour * 3600;
    min = int(ss / 60);
    ss -= min * 60;
    sec = ss;

    return 0;
  };

  std::string printShortTime() {
    stringstream name;
    string h, m, s;
    name << hour;
    h = (hour < 10 ? "0" + name.str() : name.str());
    name.str("");
    name << min;
    m = (min < 10 ? "0" + name.str() : name.str());
    name.str("");
    name << sec;
    s = (sec < 10 ? "0" + name.str() : name.str());
    name.str("");
    return h + ":" + m + ":" + s;
  };

  std::string printTime() {
    stringstream name;
    string h, m, s, ms;
    name << millisec;
    ms = name.str();
    name.str("");
    return printShortTime() + "." + ms;
  };

  int set(std::string f) {
    ParseTime(f, hour, min, sec);
    millisec = 0;
    return 0;
  };
};

class Date {
 public:
  Date() {
    time_t TIME = time(NULL);
    struct tm *tinfo = localtime(&TIME);
    year = tinfo->tm_year + 1900;
    month = tinfo->tm_mon + 1;
    day = tinfo->tm_mday;
  }
  int Set(std::string str)
  {
    ParseDate(str, year, month, day);
    return 0;
  }
  int year;
  int month;
  int day;
};

inline int ParseDate(std::string TT, Date &date) {
  int y, m, d;
  ParseDate(TT, y, m, d);
  date.year = y;
  date.month = m;
  date.day = d;
  return 0;
}

inline std::string printDate(Date &date, int format=0, bool pad=true) {
  return printDate(date.year, date.month, date.day, format, pad);
}



}

/*
int main()
{
    int m,d,y;
    cout<<printTime(0)<<endl;
   string date=printstd::string random_string(size_t length);Date(0);
   string date1=printDate(1);
   string date2=printDate(2);
   string date3=printDate(3);
   cout<<date<<endl;
   ParseDate(date,y,m,d);
   cout<<y<<"\t"<<m<<"\t"<<d<<endl;
   cout<<date1<<endl;
   ParseDate(date1,y,m,d,1);
   cout<<y<<"\t"<<m<<"\t"<<d<<endl;
   cout<<date2<<endl;
   ParseDate(date2,y,m,d,2);
   cout<<y<<"\t"<<m<<"\t"<<d<<endl;
   cout<<date3<<endl;
   ParseDate(date3,y,m,d,3);
   cout<<y<<"\t"<<m<<"\t"<<d<<endl;
   string time=" 3 : 23";
   int h,s;
   ParseTime(time, h, m, s);
   cout<<time<<"\t"<<h<<"\t"<<m<<"\t"<<s<<endl;
   time="121623";
   ParseTime(time, h, m, s);
   cout<<time<<"\t"<<h<<"\t"<<m<<"\t"<<s<<endl;
   string file="/home/trader/ca272/Pos/20140506_Inv.txt";
   cout<<"FileStamp: "<<printFileTimeStamp(file)<<endl;
}
*/
