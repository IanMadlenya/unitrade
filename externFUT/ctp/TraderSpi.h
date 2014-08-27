#pragma once

#include <memory>
#include "ThostFtdcTraderApi.h"
#include "FutHeader.h"
#include <dlfcn.h>

class CTraderSpi : public CThostFtdcTraderSpi {
 public:
  int order_ref;
  TThostFtdcFrontIDType FRONT_ID;
  TThostFtdcSessionIDType SESSION_ID;

  CTraderSpi(CThostFtdcTraderApi *m, int iR) {
    TraderApi = m;
    iTradeRequestID = iR;
  }

  int iTradeRequestID;
  /// when the server is connected while login is not finished
  virtual void OnFrontConnected();

  /// when received response after loging request
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast);

  /// when settlement information is received
  virtual void OnRspSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  /// when received response of instrument query
  virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                                  CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast);

  ///请求查询资金账户响应
  virtual void OnRspQryTradingAccount(
      CThostFtdcTradingAccountField *pTradingAccount,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  ///请求查询投资者持仓响应
  virtual void OnRspQryInvestorPosition(
      CThostFtdcInvestorPositionField *pInvestorPosition,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  ///报单录入请求响应
  virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast);

  ///报单操作请求响应
  virtual void OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  ///错误应答
  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast);

  ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
  virtual void OnFrontDisconnected(int nReason);

  ///心跳超时警告。当长时间未收到报文时，该方法被调用。
  virtual void OnHeartBeatWarning(int nTimeLapse);

  ///报单通知
  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

  ///成交通知
  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

  ///报单录入请求
  void ReqOrderInsert(std::shared_ptr<FuturesOrder>);

  void MarketOrder(std::shared_ptr<FuturesOrder>);

  void ReqSettlementPrice(std::shared_ptr<Futures>);

  ///报单操作请求
  void ReqOrderAction(std::shared_ptr<FuturesOrder>);

  virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                   CThostFtdcRspInfoField *pRspInfo);

  virtual void OnRtnFromBankToFutureByBank(
      CThostFtdcRspTransferField *pRspTransfer);
  virtual void OnRtnFromFutureToBankByBank(
      CThostFtdcRspTransferField *pRspTransfer);

  virtual void OnRtnFromBankToFutureByFutures(
      CThostFtdcRspTransferField *pRspTransfer);
  virtual void OnRtnFromFutureToBankByFutures(
      CThostFtdcRspTransferField *pRspTransfer);
  ///请求查询资金账户
  void ReqQryTradingAccount();
  ///请求查询投资者持仓
  void ReqQryInvestorPosition(char *name);

 private:
  ///用户登录请求
  void ReqUserLogin();
  ///投资者结算结果确认
  void ReqSettlementInfoConfirm();
  ///请求查询合约
  void ReqQryInstrument();

  CThostFtdcTraderApi *TraderApi;

  // 是否收到成功的响应
  bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
  // 是否我的报单回报
  bool IsMyOrder(CThostFtdcOrderField *pOrder);
  // 是否正在交易的报单
  bool IsTradingOrder(CThostFtdcOrderField *pOrder);
};

class CTPTrader {
 public:
  int iTradeRequestID;
  CTraderSpi *TRADERSPI;

  CThostFtdcTraderApi *TRADERAPI;
  int start(std::string name, std::string FRONT) {
    iTradeRequestID = 1;
    void *handler = dlopen("libthosttraderapi.so", RTLD_NOW | RTLD_LOCAL);
    if (handler == NULL) {
      std::cerr << "--->>> CTPTrader"
                << ": cannot load libthosttraderapi.so" << std::endl;
      return -1;
    }

    dlerror();
    CThostFtdcTraderApi *(*CreateTrader)(const char *);
    CreateTrader = (CThostFtdcTraderApi * (*)(const char *))dlsym(
        handler, "_ZN19CThostFtdcTraderApi19CreateFtdcTraderApiEPKc");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      std::cerr << "!--->>> Cannot load symbol: " << dlsym_error << std::endl;
    }

    TRADERAPI = (*CreateTrader)(name.c_str());
    TRADERSPI = new CTraderSpi(TRADERAPI, iTradeRequestID);
    TRADERAPI->RegisterSpi(TRADERSPI);
    // connect to quote server
    TRADERAPI->RegisterFront(const_cast<char *>(FRONT.c_str()));
    TRADERAPI->SubscribePrivateTopic(THOST_TERT_QUICK);
    TRADERAPI->SubscribePublicTopic(THOST_TERT_QUICK);
    TRADERAPI->Init();

    std::cerr << "--->>> CTPtrader"
              << ": connected with server " << std::endl;

    // TRADERAPI->Join();
  }
  void stop() {
    if (TRADERAPI) {
      TRADERAPI->RegisterSpi(NULL);
      TRADERAPI->Release();
      // delete MdApi;
    }
    if (TRADERSPI) {
      delete TRADERSPI;
    }
  }
};
