#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

int ReplaceValue(string file, string key, string value)
{
  string inbuf;
  ifstream input_file(file.c_str());
  vector<string> content;
  size_t found;
  bool match = false;
  while (!input_file.eof())
  {
      getline(input_file, inbuf);
      if (inbuf=="") continue;
      //unsigned long spot = inbuf.find(key);
      found=inbuf.find_first_not_of("\t ");
      match = true;
      unsigned int i=0;
      for (; i<key.size(); i++ )
      {
  	  	if (  inbuf[i+found] != key[i] )
  	  	{
  	  		match = false;
			break;
		}
		//cerr<<inbuf[i+found]<<"\t"<< key[i]<<"\t"<< match<<endl;

      }
      if ( inbuf[found+i] != '=' && inbuf[found+i] != ' '  )
  	  match = false;
      
      //cout<<inbuf[found+i]<<endl;
      if(match)
      {
         string tmpstring = key;
         tmpstring += " = ";
         tmpstring += value;
         tmpstring += " ;";
         inbuf = tmpstring;
      }
      content.push_back(inbuf);
  }
  input_file.close();
  ofstream output_file;
  output_file.open(file.c_str());
  for (unsigned int i=0; i<content.size(); i++)
  	output_file<<content[i]<<endl;
  output_file.close();
  return 0;
}

int ReplaceValueList(string file, vector<string> original, vector<string> value)
{
  string inbuf;
  ifstream input_file(file.c_str());
  vector<string> content;
  size_t found;
  string key;
  bool match = false;
  while (!input_file.eof())
  {
    getline(input_file, inbuf);
    if (inbuf=="") continue;
    match = false;
    for (unsigned int j=0; j<original.size();++j)
    {
      key = original[j]; 
      found=inbuf.find_first_not_of("\t ");
      size_t spot = inbuf.find(key, found);
      if (spot!=string::npos)
      {
      	if ( inbuf.size()<=found+key.size() || (inbuf[found+key.size()] != '=' && inbuf[found+key.size()] != ' ' && inbuf[found+key.size()]!=':')  )
  	  match = false;
	else
	{
	  match = true;
	  break;
	}
      }
    }  
    if(!match)
    {
	content.push_back(inbuf);
    }
  }
  for (unsigned int i=0; i<value.size();i++)
	content.push_back(value[i]);

  input_file.close();
  ofstream output_file;
  output_file.open(file.c_str());
  for (unsigned int i=0; i<content.size(); i++)
  	output_file<<content[i]<<endl;
  output_file.close();
  return 0;
}



/*
int main(int argc, char const *argv[])
{
	vector<string> original, value;
	original.push_back("pos");
	original.push_back("value");
	value.push_back("pos:30");
	value.push_back("value:40");

	value.push_back("pos:60");
	value.push_back("value:70");
	
	ReplaceValueList("demo.txt", original, value);
	return 0;
}

*/
