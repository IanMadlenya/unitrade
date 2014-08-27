#include "MdSpi.h"
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "global.h"
using namespace std;

#pragma warning(disable : 4996)

// USER_API参数
//extern CThostFtdcMdApi* pUserApi;

// 配置参数
		

//extern int iRequestID;
//extern class Condition Conditions;
static int num=0;

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
    cerr << "--->>> fut_quote:"<< __FUNCTION__ << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
    cerr << "--->>> fut_quote: " << __FUNCTION__ << endl;
    cerr << "--->>> fut_quote: Reason = " << nReason << endl;
    FileLogs->error_log <<printTime(0)<<"-[Fut Quote Connection ERROR]: Quote disconnected with errorcode "<<nReason <<endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "!-->>> fut_quote:" << __FUNCTION__ << endl;
    cerr << "!-->>> fut_quote: nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
    cerr << "--->>> fut_quote: " << __FUNCTION__ << endl;
	///用户登录请求
    ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
	strcpy(req.UserID, para.p_futuresconfig->INVESTOR_ID.c_str());
	strcpy(req.Password, para.p_futuresconfig->PASSWORD.c_str());
	int iResult = MdApi->ReqUserLogin(&req, ++iRequestID);
        cerr << "--->>> fut_quote: sending user login request: " << ((iResult == 0) ? "success" : "fail") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr << "--->>> " << __FUNCTION__ << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// 请求订阅行情
		SubscribeMarketData();	
	}
}

void CMdSpi::SubscribeMarketData()
{
    size_t num_ins = para.p_futuresconfig->INSTRUMENT_NAME_LIST.size();
    size_t len_ins = para.p_futuresconfig->INSTRUMENT_NAME_LIST[0].size();
    char **p = (char**)malloc(sizeof(char*)*num_ins);
    for (int i=0; i<num_ins; i++)
    {
        p[i] = (char*)malloc(sizeof(char)*(1+len_ins));
        std::size_t length = para.p_futuresconfig->INSTRUMENT_NAME_LIST[i].copy(p[i],len_ins);
        p[i][length]='\0';
    }
	int iResult = MdApi->SubscribeMarketData(p, (int)num_ins);
    cerr << "--->>> fut_quote: sending subsribing requests... " << ((iResult == 0) ? "success" : "fail") << endl;
    for (int i=0; i<num_ins; i++)
    {
        free(p[i]);
    }
    free(p);
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
  num++;
  cerr << "--->>> fut_quote: Subscribe information stream of "<<pSpecificInstrument->InstrumentID << endl;
  //if (num==2) // we finished subscribing information stream
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
  cerr << "--->>> fut_quote: Unsubscribe information stream of "<<pSpecificInstrument->InstrumentID << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    string name = pDepthMarketData->InstrumentID;
    CTPUpdateFuturesPrice(futures_book.instruments[name], pDepthMarketData);
    //futures_book.updateSingal();
    condition.fut_quote_flag =true;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// if ErrorID != 0, that means we are in some trouble
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
        cerr << "--->>> fut_quote: ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

int CTPUpdateFuturesPrice(Futures& fut, CThostFtdcDepthMarketDataField *md)
{
    if (md==NULL)
        return 0;
    fut.ask1 = (md->AskPrice1>10000?-1:md->AskPrice1);
    fut.bid1 = (md->BidPrice1>10000?-1:md->BidPrice1);
    fut.ask1_vol = md->AskVolume1;
    fut.bid1_vol = md->BidVolume1;
    char Time[2];
    fut.OpenPrice = md->OpenPrice;
    fut.LastPrice = ((md->LastPrice)>100000?-1:md->LastPrice);
    fut.PreSettlementPrice = md->PreSettlementPrice;
    std::cout<<md->SettlementPrice<<std::endl;
    fut.PreClosePrice = md->PreClosePrice;
    fut.HighPrice = md->HighestPrice;
    fut.LowPrice = md->LowestPrice;
    fut.AveragePrice = md->AveragePrice;
    fut.PreOpenInterest = md->PreOpenInterest;
    fut.Volume = md->Volume;
    fut.VolumeChange = fut.Volume-fut.PreVolume;
    fut.PreVolume = md->Volume;
    
    fut.OpenInterestChange = abs(md->OpenInterest - fut.OpenInterest);
    if (fut.OpenInterest == 0)
        fut.OpenInterestChange = 0;
    fut.OpenInterest = md->OpenInterest;
    fut.Turnover = md->Turnover;
    
    Time[0] = md->UpdateTime[0];
    Time[1] = md->UpdateTime[1];
    fut.hour = atoi(Time);
    Time[0] = md->UpdateTime[3];
    Time[1] = md->UpdateTime[4];
    fut.min = atoi(Time);
    Time[0] = md->UpdateTime[6];
    Time[1] = md->UpdateTime[7];
    fut.sec = atoi(Time);
    fut.milisec = md->UpdateMillisec;
    
    fut.updatePL();
    return 0;
};

int CTPUpdateFuturesPrice(std::shared_ptr<Futures> fut, CThostFtdcDepthMarketDataField *md)
{
    if (md==NULL)
        return 0;
    fut->ask1 = (md->AskPrice1>10000?-1:md->AskPrice1);
    fut->bid1 = (md->BidPrice1>10000?-1:md->BidPrice1);
    fut->ask1_vol = md->AskVolume1;
    fut->bid1_vol = md->BidVolume1;
    char Time[2];
    fut->OpenPrice = md->OpenPrice;
    fut->LastPrice = ((md->LastPrice)>100000?-1:md->LastPrice);
    fut->PreSettlementPrice = md->PreSettlementPrice;
    //std::cout<<fut->name<<md->SettlementPrice<<"\t"<<md->LastPrice<<"\t"<<fut->PreSettlementPrice <<std::endl;
    fut->PreClosePrice = md->PreClosePrice;
    fut->HighPrice = md->HighestPrice;
    fut->LowPrice = md->LowestPrice;
    fut->AveragePrice = md->AveragePrice;
    fut->PreOpenInterest = md->PreOpenInterest;
    fut->Volume = md->Volume;
    fut->VolumeChange = fut->Volume-fut->PreVolume;
    fut->PreVolume = md->Volume;
    
    fut->OpenInterestChange = abs(md->OpenInterest - fut->OpenInterest);
    if (fut->OpenInterest == 0)
        fut->OpenInterestChange = 0;
    fut->OpenInterest = md->OpenInterest;
    fut->Turnover = md->Turnover;
    
    Time[0] = md->UpdateTime[0];
    Time[1] = md->UpdateTime[1];
    fut->hour = atoi(Time);
    Time[0] = md->UpdateTime[3];
    Time[1] = md->UpdateTime[4];
    fut->min = atoi(Time);
    Time[0] = md->UpdateTime[6];
    Time[1] = md->UpdateTime[7];
    fut->sec = atoi(Time);
    fut->milisec = md->UpdateMillisec;
    
    fut->updatePL();
    return 0;
};
