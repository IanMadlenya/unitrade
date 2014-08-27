#include <iostream>
#include <dlfcn.h>
#include "MdSpi.h"



using namespace std;
int main(){

    


	CTPMd MD;




                void* handler = dlopen("libthostmduserapi.so", RTLD_NOW|RTLD_LOCAL);
                if (handler==NULL)
                {
                        std::cerr<<"--->>> CTPmd"<<": cannot load libthostmduserapi.so"<<std::endl;
                        return -1;
                }

                dlerror();
                CThostFtdcMdApi * (*CreateMd)(const char *pszFlowPath, const bool bIsUsingUdp, const bool bIsMulticast);
                CreateMd =(CThostFtdcMdApi* (*)(const char*, bool, bool))  dlsym(handler,"_ZN15CThostFtdcMdApi15CreateFtdcMdApiEPKcbb");
                const char* dlsym_error = dlerror();
                if (dlsym_error)
                {
                        std::cerr<<"!---> Cannot load symbol: "<<dlsym_error<<std::endl;
                }

	
		dlclose(handler);


	return 0;
}
