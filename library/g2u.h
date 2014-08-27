#ifndef _bin_h_g2u_ 
#define _bin_h_g2u_ 1

#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <string>
#include <iostream>

namespace bin
{


inline std::string gbk2utf(const std::string& src)
{
    unsigned long ret = 0;
    size_t inlen = src.size() + 1;
    size_t outlen = inlen*1.5;
    char* inbuf = const_cast<char*>(src.c_str());
    char* dst = (char*)malloc(sizeof(char)*outlen);
    char* outbuf = dst;
    iconv_t cd;

    // starkwong: if src==dst, the string will become invalid during conversion since UTF-8 is 3 chars in Chinese but GBK is mostly 2 chars

    cd = iconv_open("UTF-8", "GBK");
    if (cd != (iconv_t)-1) 
    {
    	ret = iconv(cd, &inbuf, &inlen, &outbuf, &outlen);
    	if (ret != 0)
    		printf("iconv failed err: %s\n", strerror(errno));
   
        iconv_close(cd);
    }
    std::string result = std::string(dst);
    free(dst);
    return result;
}

}

#endif
