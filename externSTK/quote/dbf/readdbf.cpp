#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "global.h"
#include "StkHeader.h"
#include <bitset>
#include <cstring>
#include <string>
#include <algorithm>

using namespace std;

namespace unitrade
{

unsigned int UnpackBigEndian4(unsigned char *p) {
    unsigned int ret = (unsigned int)*p |
    (unsigned int)*(p+1) << 8 |
    (unsigned int)*(p+2) << 16 |
    (unsigned int)*(p+3) << 24 ;
    return ret;
}

unsigned int UnpackBigEndian2(unsigned char *p) {
    unsigned int ret = (unsigned int)*p |
    (unsigned int)*(p+1) << 8 ;
    return ret;
}

int parseQuoteByDBF(std::shared_ptr<StockBook> stock_book, std::string shanghai_quote, std::string shenzhen_quote)
{
    int inputFd;
	ssize_t numRead;
	unsigned char buf[4];
    unsigned char cbuf[2];
    char char6[7];
    char char8[9];
    char char9[10];
    char char10[11];
    char char12[13];
    char char17[18];
    int rec_count,rec_len,h_len;
    

	/* start parsing shanghai quote files */
	inputFd = open(shanghai_quote.c_str(), O_RDONLY);
    if (inputFd == -1){
        ErrorMsg("cannot open shanghai quote file");
        return 1;
    }
	lseek(inputFd, 4, SEEK_SET);
    numRead = read(inputFd, buf, 4);
	if (numRead == -1){
		ErrorMsg("quote reading error");
		return 2;
	}
    rec_count=UnpackBigEndian4(buf);

    numRead = read(inputFd, cbuf, 2);
    if (numRead == -1){
        ErrorMsg("quote reading error");
        return 2;
    }
    h_len= UnpackBigEndian2(cbuf);

    numRead = read(inputFd, cbuf, 2);
    if (numRead == -1){
        ErrorMsg("shanghai quote reading error");
        return 2;
    }
    rec_len =UnpackBigEndian2(cbuf);
    char* rec = (char*)malloc(sizeof(char)*rec_len);
    lseek(inputFd, h_len+1, SEEK_SET);
    numRead = read(inputFd, rec, rec_len);
    StockList::iterator it;
    IndexList::iterator iti;
    
    // set shanghai quote time stamp
    ::copy(rec + 6, rec + 12, char6);
    char6[6]='\0';
    stock_book->Shanghai_quotestamp.set(string(char6));
    
    // read Shanghai quote record line by line
    for (int i=1; i < rec_count; ++i){
        numRead = read(inputFd, rec, rec_len);
        ::copy(rec, rec + 6, char6);
        char6[6]='\0';
        
        // read repo quote
        if (char6[0]=='2'&&char6[1]=='0'&&char6[2]=='4')
        {
            ::copy(rec+6, rec + 14, char8);
            char8[8]='\0';
            ::copy(rec+14, rec + 22, char8);
            it = stock_book->_RepoBook.find(string(char6));
            if (it != stock_book->_RepoBook.end())
                it->second->last_price = atof(char8);
            continue;
        }
        
        // read index quote
        if (char6[0]=='0'&&char6[1]=='0'&&char6[2]=='0')
        {
            ::copy(rec+6, rec + 14, char8);
            char8[8]='\0';
            ::copy(rec+14, rec + 22, char8);
            iti = stock_book->_IndexBook.find(string(char6));
            if (iti != stock_book->_IndexBook.end())
            {
                ::copy(rec+14, rec + 22, char8);
                iti->second->prev_close = atof(char8);
                ::copy(rec+22, rec + 30, char8);
                iti->second->open = atof(char8);
                ::copy(rec+58, rec + 66, char8);
                iti->second->last_price = atof(char8);
                iti->second->compile();
            }
            continue;
        }
        
        if (char6[0]!='6')
            continue;
        ::copy(rec+6, rec + 14, char8);
        char8[8]='\0';
        it = stock_book->_StockBook.find(string(char6));
        if (it != stock_book->_StockBook.end()){
            ::copy(rec+14, rec + 22, char8);
            it->second->prev_close = atof(char8);
            ::copy(rec+22, rec + 30, char8);
            it->second->open = atof(char8);
            ::copy(rec+30, rec + 42, char12);
            it->second->vol = atof(char12);
            ::copy(rec+42, rec + 50, char8);
            it->second->high = atof(char8);
            ::copy(rec+50, rec + 58, char8);
            it->second->low = atof(char8);
            ::copy(rec+58, rec + 66, char8);
            it->second->last_price = atof(char8);

            ::copy(rec+66, rec + 74, char8);
            it->second->bid1 = atof(char8);
            ::copy(rec+74, rec + 82, char8);
            it->second->ask1 = atof(char8);
            
            ::copy(rec+100, rec + 110, char10);
            it->second->bid1_vol = atof(char10);


            ::copy(rec+110, rec + 118, char8);
            it->second->bid2 = atof(char8);
            ::copy(rec+118, rec + 128, char10);
            it->second->bid2_vol = atof(char10);

            ::copy(rec+128, rec + 136, char8);
            it->second->bid3 = atof(char8);
            ::copy(rec+136, rec + 146, char10);
            it->second->bid3_vol = atof(char10);

            ::copy(rec+146, rec + 156, char10);
            it->second->ask1_vol = atof(char10);

            ::copy(rec+156, rec + 164, char8);
            it->second->ask2 = atof(char8);
            ::copy(rec+164, rec + 174, char10);
            it->second->ask2_vol = atof(char10);

            ::copy(rec+174, rec + 182, char8);
            it->second->ask3 = atof(char8);
            ::copy(rec+182, rec + 192, char10);
            it->second->ask3_vol = atof(char10);

            
            ::copy(rec+192, rec + 200, char8);
            it->second->bid4 = atof(char8);
            ::copy(rec+200, rec + 210, char10);
            it->second->bid4_vol = atof(char10);

            ::copy(rec+210, rec + 218, char8);
            it->second->bid5 = atof(char8);
            ::copy(rec+218, rec + 228, char10);
            it->second->bid5_vol = atof(char10);

            

            ::copy(rec+228, rec + 236, char8);
            it->second->ask4 = atof(char8);
            ::copy(rec+236, rec + 246, char10);
            it->second->ask4_vol = atof(char10);

            ::copy(rec+246, rec + 254, char8);
            it->second->ask5 = atof(char8);
            ::copy(rec+254, rec + 264, char10);
            it->second->ask5_vol = atof(char10);
            
            it->second->update();
        }
    }
    
    /* start parsing shenzhen quote files */
    inputFd = open(shenzhen_quote.c_str(), O_RDONLY);
    if (inputFd == -1){
        ErrorMsg("cannot open shenzhen quote file");
        return 1;
    }
    lseek(inputFd, 4, SEEK_SET);
    numRead = read(inputFd, buf, 4);
    if (numRead == -1){
        ErrorMsg("quote reading error");
        return 2;
    }
    rec_count=UnpackBigEndian4(buf);
    
    numRead = read(inputFd, cbuf, 2);
    if (numRead == -1){
        ErrorMsg("quote reading error");
        return 2;
    }
    h_len= UnpackBigEndian2(cbuf);
    
    numRead = read(inputFd, cbuf, 2);
    if (numRead == -1){
        ErrorMsg("shanghai quote reading error");
        return 2;
    }
    rec_len =UnpackBigEndian2(cbuf);
    char* recs = (char*)malloc(sizeof(char)*rec_len);
    lseek(inputFd, h_len+1, SEEK_SET);
    numRead = read(inputFd, recs, rec_len);
    ::copy(recs+73, recs + 79, char6);
    char6[6]='\0';
    stock_book->Shenzhen_quotestamp.set(string(char6));

    for (int i=1; i < rec_count; ++i){
        numRead = read(inputFd, recs, rec_len);
        ::copy(recs, recs + 6, char6);
        char6[6]='\0';
        if (strcmp(char6,"399001")==0 || strcmp(char6,"399005")==0 || strcmp(char6,"399006")==0)
        {
            ::copy(recs+14, recs + 23, char9);
            stock_book->_IndexBook[string(char6)]->prev_close = atof(char9);
            ::copy(recs+23, recs + 32, char9);
            stock_book->_IndexBook[string(char6)]->open = atof(char9);
            ::copy(recs+32, recs + 41, char9);
            stock_book->_IndexBook[string(char6)]->last_price = atof(char9);
            stock_book->_IndexBook[string(char6)]->compile();
        }
        
        it = stock_book->_StockBook.find(string(char6));
        if (it != stock_book->_StockBook.end()){
            ::copy(recs+14, recs + 23, char9);
            it->second->prev_close = atof(char9);
            ::copy(recs+23, recs + 32, char9);
            it->second->open = atof(char9);
            ::copy(recs+32, recs + 41, char9);
            it->second->last_price = atof(char9);
            ::copy(recs+53, recs + 70, char17);
            it->second->vol = atof(char17);
            ::copy(recs+79, recs + 88, char9);
            it->second->high = atof(char9);
            ::copy(recs+88, recs + 97, char9);
            it->second->low = atof(char9);
            ::copy(recs+141, recs + 150, char9);
            it->second->ask5 = atof(char9);
            ::copy(recs+150, recs + 162, char12);
            it->second->ask5_vol = atof(char12);
            ::copy(recs+162, recs + 171, char9);
            it->second->ask4 = atof(char9);
            ::copy(recs+171, recs + 183, char12);
            it->second->ask4_vol = atof(char12);
            ::copy(recs+183, recs + 192, char9);
            it->second->ask3 = atof(char9);
            ::copy(recs+192, recs + 204, char12);
            it->second->ask3_vol = atof(char12);
            ::copy(recs+204, recs + 213, char9);
            it->second->ask2 = atof(char9);
            ::copy(recs+213, recs + 225, char12);
            it->second->ask2_vol = atof(char12);
            ::copy(recs+225, recs + 234, char9);
            it->second->ask1 = atof(char9);
            ::copy(recs+234, recs + 246, char12);
            it->second->ask1_vol = atof(char12);

            ::copy(recs+246, recs + 255, char9);
            it->second->bid1 = atof(char9);
            ::copy(recs+255, recs + 267, char12);
            it->second->bid1_vol = atof(char12);
  
            ::copy(recs+267, recs + 276, char9);
            it->second->bid2 = atof(char9);
            ::copy(recs+276, recs + 288, char12);
            it->second->bid2_vol = atof(char12);
            
            ::copy(recs+288, recs + 297, char9);
            it->second->bid3 = atof(char9);
            ::copy(recs+297, recs + 309, char12);
            it->second->bid3_vol = atof(char12);
            
            ::copy(recs+309, recs + 318, char9);
            it->second->bid4 = atof(char9);
            ::copy(recs+318, recs + 330, char12);
            it->second->bid4_vol = atof(char12);
            
            ::copy(recs+330, recs + 339, char9);
            it->second->bid5 = atof(char9);
            ::copy(recs+339, recs + 351, char12);
            it->second->bid5_vol = atof(char12);
            
            it->second->update();
        }
    }
    free(rec);
    free(recs);
    return 0;
}

}
