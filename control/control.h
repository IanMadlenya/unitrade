#ifndef _h_control_
#define _h_control_ 1

#include <iostream>
#include <string>
#include <memory>
#include <iomanip>
#include "ssl_conversation.h"

using namespace std;

namespace bincon {


/*!
 * Self-explanatory structure describing command format and helps.
 * */
typedef struct CommandFormat
{
 public:
  std::string Major{""};
  std::string Minor{""};
  std::vector<std::string> Arguments;
  std::string Help{""};
  bool hasSubCmd {false};
  int arguments_min {};
  int arguments_max {};
  CommandFormat(std::string c1, std::string c2)
  {
    Major = c1;
    Minor = c2;
    if (Minor!="")
      hasSubCmd = true;
  }
  int Print()
  {
    std::cout<<std::setw(13)<<Major;
    std::cout<<std::setw(13)<<Minor;
    for (auto i:Arguments)
      std::cout<<std::setw(9)<<i;
    std::cout<<std::endl;
    return 0;
  }
} CommandFormat;


typedef std::vector<CommandFormat> CommandFormatList;


inline int PrintCommandFormatList(CommandFormatList &command_book)
{
  std::cout<<std::setw(13)<<"[MajorCMD]"<<std::setw(13)<<"[MinorCMD]"<<std::setw(9)<<"[Argus]"<<std::endl;
  std::cout<<"------------------------------------"<<std::endl;
  for (auto i:command_book)
  {
    i.Print();
  }
  return 0;
};


inline int ParseCommandFormat(std::ifstream& ifs, CommandFormatList& command_book) {
  std::string line;
  std::vector<std::string> vs;
  bool header = false;
  while (getline(ifs, line)) 
  {
    if (bin::CheckComment(line)) continue;
    if (header) {
      header = false;
      continue;
    }
    vs = bin::SplitLine(line);
    if (vs.size() == 1) {
      std::cerr << "--->>> ParseCommandFormat: " << vs[0]
                << " format not compatible, ignored" << std::endl;
      continue;
    }

    std::string major = vs[0];
    std::string minor = "";
    if (vs[1][0] != '<' && vs[1][0] != '[' && vs[1][0] != '{')
      minor = vs[1];

    for (auto i : command_book) {
      if (i.Major == major && i.Minor == minor) {
        std::cerr << "--->>> ParseCommandFormat: duplicate for " << major << " "
                  << minor << ", ignored" << std::endl;
        goto leave;
      }
    }
    command_book.push_back(CommandFormat(major, minor));
    // command_book.back().Arguments.push_back();
    for (unsigned int i=1; i<vs.size();++i)
    {
      if (vs[i][0]=='<'||vs[i][0]=='[')
        command_book.back().Arguments.push_back(vs[i]);
    }

  leave:
    ;
  }
  return 0;
}


inline int ParseCommandFormat(std::string name, CommandFormatList& command_book) {
  std::ifstream ifs;
  ifs.open(name);
  if (!ifs.is_open())
  {
    std::cerr<<"--->>> ParseCommandFormat: cannot open file="<<name<<std::endl;
    return -1;
  }
  return ParseCommandFormat(ifs, command_book);
  
};


inline int VerifyCommand(std::vector<string> vs, CommandFormat& packet)
{
  /*!
   *   Purpose: parse input shell command to MsgPacket
   *   Params: "vs" is the incoming message stream
   *   Return: generated commandpacket, shall be RVO  
   */

  unsigned int start = 1 + packet.Arguments.size();
  if (packet.hasSubCmd) start++;
  if (vs.size()> start)
    return -1;
  return 0;
}



inline bin::CommandPacket GenerateCommandPacket(std::vector<string> vs, CommandFormat& packet)
{
  /*!
   *   Purpose: parse input shell command to MsgPacket
   *   Params: "vs" is the incoming message stream
   *   Return: generated commandpacket, shall be RVO  
   */
  std::string minor = "";
  unsigned int start = 1;
  unsigned int mark = 0;
  if (packet.hasSubCmd) { minor = vs[1]; start = 2; }
  bin::CommandPacket command;

  command.SetCommand(vs[0], minor);
  command.hasSubCmd = packet.hasSubCmd;
  for (;start<vs.size();++start,++mark)
  {
    command.argumentlist[packet.Arguments[mark]] = vs[start];
  }
  return command;
}



class Shell {

 public:
  std::string command;
  bin::CommandPacket command_packet;
  CommandFormatList _command_book;
  std::string hash {""}; 
  binsec::SSLStream *sslstream;
  std::string remote_ans {""};

  Shell(binsec::SSLStream *s){sslstream=s;}
  Shell(binsec::SSLStream *s, std::string h){sslstream=s;hash=h;}

  int GetCommand() 
  {
    cout << " Controller >> ";
    if (cin.fail())
    {
      std::cout<<std::endl;
      exit(1);
    }
    getline(cin, command);
    /*<should not be case sensitive>*/
    command = bin::ToLower(command);
    /*<should be trimmed>*/
    bin::trim(command);
    if (command=="")
      return -1; //  empty command
    return 0; 
  }


  int Run(const CommandFormatList &command_book) 
  {
    sslstream->silence(true);
    _command_book = command_book;
    cout << "\n----------------- Unitrade Controller--------------" << endl;
    cout << " help <command> will show help info " << endl;
    int status;
    while (1) 
    {
      command_packet.Reset();
      if (GetCommand()==-1)
      {
        cout<<" ERROR in reading command"<<endl;
        continue;
      }

      status = ParseCommand();
      if (status==-2)
        break;
      else if (status==2)
      {
        cout<<" ERROR: command not compatible, more arguments than expected"<<endl;
        continue;
      }
      else if (status==-1)
      {
        cout<<" ERROR: command not found"<<endl;
        continue;
      } 
      status = sslstream->stream_ask(CompileCommandPacket(command_packet), remote_ans);
      auto ret = bin::ParseTokenFromString(remote_ans, false);
      cout<<remote_ans<<endl;
      for (auto i:ret)
      {
        cout<<" "<<i.first<<": "<<i.second<<endl;
      }
      if (status==-1 && status == -2)
      {
        cout<<" ERROR: connection error"<<endl;
        return status;
      }
    }

    return 0;
  }


  int ParseCommand()
  {
    int status=-1; /*-2 quit, -1 not found, 0 found, 1 not recognized, 2 not compatible*/
    if (command=="help"||command=="h")
    {
      PrintCommandFormatList(_command_book);
      status = 0;
    }
    else if (command=="quit"||command=="q"||command=="exit")
      status = -2;
    else if (command=="cls"|| command=="clear")
    {
      std::system("clear");
      status = 0 ;
    }
    else
    {
      auto vs = bin::SplitLine(command);
      if (vs.size()==0) return 0; /*empty command*/
      for (auto i:_command_book)
      {
        if (i.Major == bin::trim(vs[0])) /*major command matches*/
        {
          status = 1;
          if (i.hasSubCmd && vs.size()>1 && i.Minor==bin::trim(vs[1]) ) /*minor command matches*/ 
          {

            if (VerifyCommand(vs, i)==-1)
              return 2;
            command_packet = GenerateCommandPacket(vs, i);
            command_packet.auth = hash;
            //command_packet.Print();
            status = 0;
          }
          else if (!i.hasSubCmd) /* only major command*/
          {
            if (VerifyCommand(vs,i)==-1)
              return 2;
            command_packet = GenerateCommandPacket(vs, i);
            command_packet.auth = hash;
            //command_packet.Print();
            status = 0;
          }
        }
      }

      if (status == 1)
      {
        cout<<" ERROR: Command not complete"<<endl;
        cout<<" Possible Usage: "<<endl;
        for (auto i:_command_book)
        {
          if (i.Major == bin::trim(vs[0]))
            i.Print();
        }
      }
    }
   
    return status; /*not found*/
  }


  void Help(const std::string arg = "") const {}

};


}
#endif
