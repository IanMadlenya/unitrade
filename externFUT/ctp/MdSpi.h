#pragma once
#include "ThostFtdcMdApi.h"
#include <stdlib.h>
#include <string.h>
#include "FutHeader.h"
#include <dlfcn.h>

int CTPUpdateFuturesPrice(Futures& fut, CThostFtdcDepthMarketDataField *md);
int CTPUpdateFuturesPrice(std::shared_ptr<Futures> fut, CThostFtdcDepthMarketDataField *md);



class CMdSpi : public CThostFtdcMdSpi
{
public:
	int iRequestID;
	CMdSpi(CThostFtdcMdApi* m, int iR){
		MdApi = m;
		iRequestID = iR;
	}
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);
		
	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
	void ReqUserLogin();
	void SubscribeMarketData();
	CThostFtdcMdApi* MdApi;
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
};


class CTPMd{
public:
	int iRequestID;
	CMdSpi* MDSPI;

	CThostFtdcMdApi* MDAPI;
	int start(std::string name, std::string FRONT)
	{
		iRequestID=1000;
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
		//if (CreateMd==NULL)
		//{
		//	std::cerr<<"--->>> CTPmd: cannot load CreateFtdcMdApi function, does its signature changed?"<<std::endl;
		//	return -1;
		//}

    		//MDAPI = CThostFtdcMdApi::CreateFtdcMdApi(name.c_str());
    		MDAPI = (*CreateMd)(name.c_str(), false, false);
    		MDSPI = new CMdSpi(MDAPI, iRequestID);
    		MDAPI->RegisterSpi(MDSPI);
    		// connect to quote server
         	MDAPI->RegisterFront( const_cast<char *>(FRONT.c_str()) );
             	MDAPI->Init();
                 
                      
                std::cerr << "--->>> CTPmd"<<": connected with server "  << std::endl;
        	//stop();
	        MDAPI->Join();
	
	}
	void stop(){
		if (MDAPI)
		{
	   		MDAPI->RegisterSpi(NULL);
	   		MDAPI->Release();
	   		//delete MdApi;
	   		MDAPI=NULL;
	   	}
	        if (MDSPI)
	        {
	      		delete MDSPI;
	  	}
	   //
	}
};


