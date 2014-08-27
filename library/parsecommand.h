#ifndef _bin_parsecommand_
#define _bin_parsecommand_ 1

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include "bin.h"
#include <locale>
#include <unordered_map>
using namespace std;

namespace bin {

inline std::string Normalize(std::string data) {
  if (data.size() == 0) return "";
  std::transform(data.begin(), data.end(), data.begin(), ::tolower);
  data[0] = ::toupper(data[0]);
  return data;
};

enum TaskTimeType { OnStart, OnStop, OnTime, Now };

typedef struct CommandPacket {
  std::string auth{""};
  int auth_len{};
  bool isAuth{false};
  std::string major{""};
  std::string minor{""};
  int num_argument{0};
  bool hasSubCmd{false};
  bin::Timer mark;
  TaskTimeType time_type;
  std::string code;
  std::string answer{""};
  std::map<std::string, std::string> argumentlist;
  CommandPacket(const std::string str = "") {
    auth = str;
    auth_len = str.size();
  }
  int SetCommand(std::string mj, std::string mi = "") {
    major = mj;
    if (mi != "") hasSubCmd = true;
    minor = mi;
    code = Normalize(major) + Normalize(minor);
    return 0;
  }
  int Reset()
  {
    major = "";
    minor = "";
    num_argument = 0;
    hasSubCmd = false;
    code = "";
    answer = "";
    argumentlist.clear();
    return 0;
  }
  void Print() {
    cerr << "AUTH: " << auth << endl;
    cerr << "AUTH_len: " << auth_len << endl;
    cerr << "CMD: " << major << endl;
    cerr << "Minor: " << minor << endl;
    cerr << "Code: " << code << endl;
    for (auto i : argumentlist) cerr << i.first << ": " << i.second << endl;
  }
} MsgPacket;

inline bool CheckComment(std::string& line) {
  bin::ltrim(line);
  return (line.size() == 0) || (line[0] == '#') || (line[0] == '!') ||
         (line[0] == '/') || (line[0] == '[');
}

inline std::string CompileCommandPacket(MsgPacket& packet) {
  /*!
   *   Purpose: parse message to MsgPacket
   *   Params: "message" is the incoming message stream
   *   Return: -1 if unsuccessful, otherwise positive integer denoting argument
   * number
   */
  string message;

  if (packet.auth != "") message += "auth=" + packet.auth;

  message += "&major=" + packet.major;
  if (packet.hasSubCmd) message += "&minor=" + packet.minor;
  message += "&code=" + packet.code;
  for (auto i : packet.argumentlist) message += "&" + i.first + "=" + i.second;
  return message;
};

inline int ParseCommandPacket(const std::string message, MsgPacket& packet) {
  /*!
   *   Purpose: parse message to MsgPacket
   *   Params: "message" is the incoming message stream
   *   Return: -1 if unsuccessful, otherwise positive integer denoting argument
   * number
   */

  if (message.size() == 0) return -1;

  string content, head;
  unsigned int i, j, k, num;
  num = 0;
  i = j = k = 0;  // i marks head, j marks '=', k marks '&' or '#'

  do {
    j = i + 1;
    while (message[j] != '=' && j < message.size() - 1) j++;

    k = j + 1;
    while (message[k] != '&' && k < message.size() - 1) k++;

    if (k <= message.size() - 1) {
      head = message.substr(i, j - i);
      if (message[k] == '&')
        content = message.substr(j + 1, k - j - 1);
      else
        content = message.substr(j + 1, k - j);
      i = k + 1;

      if (head == "auth" || head == "Auth" || head == "AUTH") {
        packet.auth = content;
        packet.auth_len = content.size();
      } else if (bin::trim(head) == "major")
        packet.major = content;
      else if (bin::trim(head) == "minor")
        packet.minor = content;
      else if (bin::trim(head) == "code")
        packet.code = content;
      else {
        if (packet.argumentlist.find(head) == packet.argumentlist.end())
          packet.argumentlist[head] = content;
      }
    } else
      i = k + 1;
  } while (i < message.size() - 1);

  return num;
};

inline std::unordered_map<std::string, std::string> ParseTokenFromString(
    const std::string message, bool reverse=true) {
  /*!
   *   Purpose: parse message to MsgPacket
   *   Params: "message" is the incoming message stream
   *   Return: -1 if unsuccessful, otherwise positive integer denoting argument
   * number
   */
  std::unordered_map<std::string, std::string> ret;
  std::unordered_map<std::string, std::string>::iterator it;
 
  if (message.size() == 0) return ret ;

  string content, head;
  unsigned int i, j, k;
 
  i = j = k = 0;  // i marks head, j marks '=', k marks '&' or '#'
 do {
    j = i + 1;
    while (message[j] != '=' && j < message.size() - 1) j++;

    k = j + 1;
    while (message[k] != '&' && k < message.size() - 1) k++;

    if (k <= message.size() - 1) {
      head = message.substr(i, j - i);
      if (message[k] == '&')
        content = message.substr(j + 1, k - j - 1);
      else
        content = message.substr(j + 1, k - j);
      i = k + 1;
      if ( (it = ret.find(head)) == ret.end()) {
        ret[head] = content;
      }
    } else
      i = k + 1;
  } while (i < message.size() - 1);
   std::unordered_map<std::string, std::string> ret_rev;
  
  if (!reverse)
  {
   for (auto i:ret)
     ret_rev[i.first]=i.second;
   return ret_rev;
  }
  return ret;
};

inline std::string ToLower(std::string data) {
  std::transform(data.begin(), data.end(), data.begin(), ::tolower);
  return data;
};
}

#endif
