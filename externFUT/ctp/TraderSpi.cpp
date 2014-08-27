#include <iostream>
#include "ThostFtdcTraderApi.h"
#include "TraderSpi.h"
#include "bin.h"
#include "global.h"
using namespace std;

#pragma warning(disable : 4996)

// USER_API parameters
extern CThostFtdcTraderApi* TraderApi;

// set parameters


// Dialogue Parameters



static int num=0;

static TThostFtdcOrderRefType	ORDER_REF;
int posquery=0;

void CTraderSpi::OnFrontConnected()
{
	cerr << "--->>> fut_execute: Front Server Connected" << endl;
	///user login request
	ReqUserLogin();
}

void CTraderSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
	strcpy(req.UserID, para.p_futuresconfig->INVESTOR_ID.c_str());
	strcpy(req.Password, para.p_futuresconfig->PASSWORD.c_str());
	int iResult = TraderApi->ReqUserLogin(&req, ++iTradeRequestID);
	cerr << "--->>> fut_execute: User Login Request Result: " << ((iResult == 0) ? "Success" : "Failure") << endl;
}


void CTraderSpi::OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField *pRspTransfer)
{
	if(pRspTransfer->TransferStatus == THOST_FTDC_TRFS_Normal)
	{
	        futures_book.cashin += pRspTransfer->TradeAmount;
            futures_book.cash += pRspTransfer->TradeAmount;
       		cerr<<"--->>> CashTransfer: ["<<wall_clock.printTime()<<"]"<<endl;
            cerr<<"--->>> CashTransfer: Transfer-Bank-To-Futures-ByBank Succeed--"<<pRspTransfer->TradeAmount<< endl;
            cerr<<"--->>> CashTransfer: Customer Fee: "<<pRspTransfer->CustFee<< endl;
            cerr<<"--->>> CashTransfer: Broker Fee: "<<pRspTransfer->BrokerFee<< endl;
	}

}
void CTraderSpi::OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField *pRspTransfer)
{
	if(pRspTransfer->TransferStatus == THOST_FTDC_TRFS_Normal)
	{
	        futures_book.cashin -= pRspTransfer->TradeAmount;
            futures_book.cash -= pRspTransfer->TradeAmount;

      		cerr<<"--->>> CashTransfer: ["<<wall_clock.printTime()<<"]"<<endl;
            cerr<<"--->>> CashTransfer: Transfer-Futures-To-Bank-ByBank Succeed--"<<pRspTransfer->TradeAmount<< endl;
            cerr<<"--->>> CashTransfer: Customer Fee: "<<pRspTransfer->CustFee<< endl;
            cerr<<"--->>> CashTransfer: Broker Fee: "<<pRspTransfer->BrokerFee<< endl;
	}
}

void CTraderSpi::OnRtnFromBankToFutureByFutures(CThostFtdcRspTransferField *pRspTransfer)
{
	if(pRspTransfer->TransferStatus == THOST_FTDC_TRFS_Normal)
	{
	        futures_book.cashin += pRspTransfer->TradeAmount;
            futures_book.cash += pRspTransfer->TradeAmount;
       		cerr<<"--->>> CashTransfer: ["<<wall_clock.printTime()<<"]"<<endl;

		cerr<<"--->>> CashTransfer: Transfer-Bank-To-Futures-ByFutures Succeed--"<<pRspTransfer->TradeAmount<< endl;
		cerr<<"--->>> CashTransfer: Customer Fee: "<<pRspTransfer->CustFee<< endl;
		cerr<<"--->>> CashTransfer: Broker Fee: "<<pRspTransfer->BrokerFee<< endl;
	}

}
void CTraderSpi::OnRtnFromFutureToBankByFutures(CThostFtdcRspTransferField *pRspTransfer)
{
	if(pRspTransfer->TransferStatus == THOST_FTDC_TRFS_Normal)
	{
        futures_book.cashin -= pRspTransfer->TradeAmount;
        futures_book.cash -= pRspTransfer->TradeAmount;
        cerr<<"--->>> CashTransfer: ["<<wall_clock.printTime()<<"]"<<endl;
		cerr<<"--->>> CashTransfer: Transfer-Futures-To-Bank-ByFutures Succeed--"<<pRspTransfer->TradeAmount<< endl;
		cerr<<"--->>> CashTransfer: Customer Fee: "<<pRspTransfer->CustFee<< endl;
		cerr<<"--->>> CashTransfer: Broker Fee: "<<pRspTransfer->BrokerFee<< endl;
	}
}


void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> fut_execute: OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// save dialogue parameters
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
        	order_ref = atoi(pRspUserLogin->MaxOrderRef);
        	order_ref++;
		
		// get current trading day
		cerr << "--->>> fut_execute: Get Current Trading day = " << TraderApi->GetTradingDay() << endl;
		/// Investor settlement confirm
		ReqSettlementInfoConfirm();
	}
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
	strcpy(req.InvestorID, para.p_futuresconfig->INVESTOR_ID.c_str());
	int iResult = TraderApi->ReqSettlementInfoConfirm(&req, ++iTradeRequestID);
	cerr << "--->>> fut_execute: Settlement Request " << ((iResult == 0) ? "Success" : "Failure") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> fut_execute: OnRspSettlementInfoConfirm" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		/// request to check instruments
	  num = 0 ;
	  ReqQryInstrument();
	}
}

void CTraderSpi::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	int iResult;
	strcpy(req.InstrumentID, para.p_futuresconfig->INSTRUMENT_NAME_LIST[num].c_str());
	iResult = TraderApi->ReqQryInstrument(&req, ++iTradeRequestID);
	cerr << "--->>> fut_execute: Checking instrument "<<para.p_futuresconfig->INSTRUMENT_NAME_LIST[num];
	switch(iResult){
	case(0):
	  cerr<<"...";
	  break;
	case(-1):
	  cerr<<": ERROR (Network unreachable)"<<endl;
	  abort();
	  break;
	case(-2):
	  cerr<<": ERROR (Too many open requests)"<<endl;
	  abort();
	  break;
	case(-3):
	  cerr<<": ERROR (Too many requests)"<<endl;
	  abort();
	  break;
	}	    
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	if (pInstrument == NULL){
		cerr<<"!-->>> fut_execute ERROR: cannot get instrument information"<<endl;
		abort();
	}
	cerr<<(pInstrument->IsTrading?"OK":"ERROR")<<endl;		
	num++;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
	  //num++;
      sleep(2);
	  if (num < para.p_futuresconfig->NUM_INSTRUMENT)
	    ReqQryInstrument();
	  else if (num==para.p_futuresconfig->NUM_INSTRUMENT)
	    ReqQryTradingAccount(); 
	}
	  //if all instruments are set, go on to further check account
}

void CTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
	strcpy(req.InvestorID, para.p_futuresconfig->INVESTOR_ID.c_str());
	int iResult = TraderApi->ReqQryTradingAccount(&req, ++iTradeRequestID);
    cerr << "--->>> fut_execute: Trading Account Query Request: " << ((iResult == 0) ? "Sent" : "Failed") << endl;
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> fut_execute: OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
        // test our balance
        cerr<<"--->>> Report: Exchange Futures Balance  = "<<pTradingAccount->PreBalance+pTradingAccount->CloseProfit-pTradingAccount->Commission+pTradingAccount->Deposit - pTradingAccount->Withdraw <<endl;
	cerr<<"--->>> Report: Local says balance = "<<futures_book.cash;
	double inconsistent = pTradingAccount->PreBalance+pTradingAccount->CloseProfit-pTradingAccount->Commission+pTradingAccount->Deposit-pTradingAccount->Withdraw - futures_book.cash;
        if ( fabs(inconsistent) < 0.1 )
        {
            cerr<<" inconsistent="<<inconsistent<<" good"<<endl;
        }
        else
        {
            cerr<<" Balance not consistent ["<<inconsistent<<"], warning log issued, now updating records...";
            futures_book.cash = pTradingAccount->PreBalance+pTradingAccount->CloseProfit-pTradingAccount->Commission+pTradingAccount->Deposit-pTradingAccount->Withdraw ;
            FileLogs->error_log<<printTime(0)<<"-[FutureHedge Cash Check Warning]: exchange report and system record are not consistent ["<<inconsistent<<"]"<<endl;
            cerr<<"success"<<endl;
        }
        
        ReqQryInvestorPosition(const_cast<char*>(para.p_futuresconfig->INSTRUMENT_NAME_LIST[0].c_str()));
	}
}


void CTraderSpi::ReqQryInvestorPosition(char *name)
{
    CThostFtdcQryInvestorPositionField req1;
    memset(&req1, 0, sizeof(req1));
    //memset(&req2, 0, sizeof(req2));
    strcpy(req1.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
    strcpy(req1.InvestorID,para.p_futuresconfig->INVESTOR_ID.c_str());
    strcpy(req1.InstrumentID, name);
    sleep(2);
    int iResult = TraderApi->ReqQryInvestorPosition(&req1, ++iTradeRequestID);
    cerr << "--->>> fut_execute: "<<req1.InstrumentID<<" Position Query Request: " << ((iResult == 0) ? "SENT" : "FAILED") << endl;
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{       
    cerr << "--->>> " << "fut_execute: OnRspQryInvestorPosition: "<< endl;

    if (bIsLast && !IsErrorRspInfo(pRspInfo))
    {
        if (pInvestorPosition!=NULL && pInvestorPosition->Position!=0 )
        {
            int ss;
            ss = (pInvestorPosition->PosiDirection=='3'?-1:1);
            cerr<<"--->>> System Report: position cost="<<pInvestorPosition->PositionCost/pInvestorPosition->Position/300<<endl;
            cerr<<"--->>> System Report: "<<pInvestorPosition->InstrumentID<<"  Pos="<<ss*pInvestorPosition->Position<<" record says:"<<futures_book.instruments[pInvestorPosition->InstrumentID]->Pos;
            if (ss*pInvestorPosition->Position==futures_book.instruments[pInvestorPosition->InstrumentID]->Pos)
                cerr<<"  good"<<endl;
            else
            {
                cerr<<"  ERROR! updating records...";
                futures_book.instruments[pInvestorPosition->InstrumentID]->Pos = ss*pInvestorPosition->Position;
                cerr<<"  success";
            }

            if (futures_book.instruments[pInvestorPosition->InstrumentID]->AV == 0  )
            {
                cerr<<"--->>> System Report: "<<pInvestorPosition->InstrumentID<<" cost is not consisitent with records, updating..."<<endl;
                futures_book.instruments[pInvestorPosition->InstrumentID]->AV = pInvestorPosition->PositionCost/pInvestorPosition->Position/300;
            }
	  futures_book.instruments[pInvestorPosition->InstrumentID]->AV = pInvestorPosition->PositionCost/pInvestorPosition->Position/300;
 
 	    
        }
	else if (pInvestorPosition==NULL && futures_book.instruments[para.p_futuresconfig->INSTRUMENT_NAME_LIST[posquery]]->Pos!=0){
		cerr<<"--->>> System warning: inconsistent with position for "<<para.p_futuresconfig->INSTRUMENT_NAME_LIST[posquery] <<endl;
		cerr<<"--->>> System warning: will update position record"<<endl;
		futures_book.instruments[para.p_futuresconfig->INSTRUMENT_NAME_LIST[posquery]]->Pos = 0;
	}

        if (posquery!=2)
        {
            sleep(2);
            posquery++;
            ReqQryInvestorPosition(const_cast<char *>(para.p_futuresconfig->INSTRUMENT_NAME_LIST[posquery].c_str()));
        }
        else
        {
             condition.fut_execute_flag=1;
             std::cout<<"--->>> fut_execute Thread is alive"<<std::endl;
        }
    }
}



void CTraderSpi::ReqOrderInsert(std::shared_ptr<FuturesOrder> TO)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req)); //initilize order
	/// Broker ID
	strcpy(req.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
	/// Investor ID
	strcpy(req.InvestorID, para.p_futuresconfig->INVESTOR_ID.c_str());
    /// User ID
    strcpy(req.UserID, para.p_futuresconfig->INVESTOR_ID.c_str());

    /// Instrument
    strcpy(req.InstrumentID, TO->InstrumentName.c_str());
    /// Order Reference

    sprintf(ORDER_REF, "%012d", order_ref);

    strcpy(req.OrderRef, ORDER_REF);
    TO->ID = order_ref;
    TO->OrderRefList.push_back(order_ref);
    cout<<"ORDER REFERENCE: "<<ORDER_REF<<endl;

    req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;



    if (TO->Direction == "sell")
    {
        req.Direction = THOST_FTDC_D_Sell;
    }
    else if (TO->Direction=="buy")
    {
        req.Direction = THOST_FTDC_D_Buy;
    }

    if (TO->Offset == "open")
    {
        strcpy(req.CombOffsetFlag , "0");
    }
    else if (TO->Offset=="close")
    {
        strcpy(req.CombOffsetFlag , "1");
    }


    strcpy(req.GTDDate, "");
    req.LimitPrice = TO->LimitPrice;
    /// Volume
    req.VolumeTotalOriginal = TO->Number;
    strcpy(req.CombHedgeFlag, para.p_futuresconfig->HEDGE_FLAG.c_str());
    ///触发条件: 立即
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///强平原因: 非强平
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///有效期类型: 当日有效
    req.TimeCondition = THOST_FTDC_TC_GFD;
    ///成交量类型: 任何数量
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///最小成交量: 1
    req.MinVolume = 1;
    ///自动挂起标志: 否
    req.IsAutoSuspend = 0;
	///价格
    //req.LimitPrice = TO->LimitPrice;
	///用户强评标志: 否
	req.UserForceClose = 0;
    int iResult = TraderApi->ReqOrderInsert(&req, iTradeRequestID);
    cerr << "--->>> Limit Order Request: " << ((iResult == 0) ? "SENT" : "Failed") << endl;
    TO->Status = 6; // revise local order status to be 'sent'
    FileLogs->futures_order_detail <<wall_clock.printTime()
                   <<setw(15)<<req.OrderRef
                   <<setw(10)<<req.InstrumentID
                   <<setw(7)<<(req.Direction==THOST_FTDC_D_Sell?"sell":"buy")
                   <<setw(7)<<(req.CombOffsetFlag[0]=='0'?"open":"close")
                   <<setw(10)<< req.LimitPrice
                   <<setw(7)<< req.VolumeTotalOriginal
                   <<setw(7)<< 0
                    <<setw(8)<<TO->OrderMode
                   <<setw(22)<<"limit-initiated"
                   <<endl;
    order_ref++;

}


void CTraderSpi::MarketOrder(std::shared_ptr<FuturesOrder> TO)
{
    CThostFtdcInputOrderField req;
    memset(&req, 0, sizeof(req)); //initilize order

    strcpy(req.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
    strcpy(req.InvestorID, para.p_futuresconfig->INVESTOR_ID.c_str());
    strcpy(req.UserID, para.p_futuresconfig->INVESTOR_ID.c_str());
    strcpy(req.InstrumentID, TO->InstrumentName.c_str());
    sprintf(ORDER_REF, "%012d", order_ref);

    strcpy(req.OrderRef, ORDER_REF);
    TO->ID = order_ref; //revise order ID
    TO->OrderRefList.push_back(order_ref);
    TO->Status = 6; // revise local order status to be 'sent'
    order_ref++;
    cerr<<"--->>> fut_execute: REF="<<ORDER_REF<<endl;
    req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
    if (TO->Offset == "open")
    {
        strcpy(req.CombOffsetFlag , "0");
    }
    else if (TO->Offset=="close")
    {
        strcpy(req.CombOffsetFlag , "1");
    }


    if (TO->Direction == "sell")
    {
        req.Direction = THOST_FTDC_D_Sell;
    }
    else if (TO->Direction=="buy")
    {
        req.Direction = THOST_FTDC_D_Buy;
    }

    strcpy(req.CombHedgeFlag, para.p_futuresconfig->HEDGE_FLAG.c_str());
    strcpy(req.GTDDate, "");
    /// Volume
    req.VolumeTotalOriginal = TO->Number;
    ///触发条件: 立即
    req.ContingentCondition = THOST_FTDC_CC_Immediately;
    ///强平原因: 非强平
    req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    ///有效期类型: 当日有效
    req.TimeCondition = THOST_FTDC_TC_IOC;
    ///成交量类型: 任何数量
    req.VolumeCondition = THOST_FTDC_VC_AV;
    ///最小成交量: 1
    req.MinVolume = 1;
    ///自动挂起标志: 否
    req.IsAutoSuspend = 0;
    req.LimitPrice = 0;
    ///用户强评标志: 否
    req.UserForceClose = 0;
    int iResult = TraderApi->ReqOrderInsert(&req, iTradeRequestID);
    cerr << "--->>> MARKET order Request: " << ((iResult == 0) ? "SENT" : "Failed") << endl;
    TO->OrderMode = "Market";
    FileLogs->futures_order_detail <<wall_clock.printTime()
                   <<setw(15)<< req.OrderRef
                   <<setw(10)<<req.InstrumentID
                   <<setw(7)<< (req.Direction==THOST_FTDC_D_Sell?"sell":"buy")
                   <<setw(7)<< (req.CombOffsetFlag[0]=='0'?"open":"close")
                   <<setw(10)<< "Market"
                   <<setw(7)<< req.VolumeTotalOriginal
                   <<setw(7)<< 0
                   <<setw(8)<<TO->OrderMode
                   <<setw(22)<<"market-initiated"
                   << endl;
    		cerr<<"--->>> fut_execute: Market Order placed--"<<TO->InstrumentName.c_str()<<"  "<<TO->Offset<<
          "  "<<TO->Number<<endl;
}




void CTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{
    cerr<<"--->>> ERROR in Order Placement "<<endl;
    for (int i=0; i<futures_order.AllOrders.size(); i++){
        if ( futures_order.AllOrders[i]->id == atoi(pInputOrder->OrderRef) ){
            futures_order.AllOrders[i]->EXError = pRspInfo->ErrorID;
            strcpy( futures_order.AllOrders[i]->EXErrorString, pRspInfo->ErrorMsg);
            cerr<<(futures_order.AllOrders[i]->EXErrorString)<<endl;
            futures_order.AllOrders[i]->Status = 200;
            //trade_engine.InRound=false;
            //trade_engine.Status =false;
        }
    }
    FileLogs->error_log<<printTime(0)<<"-[Futures Order ERROR-EXCHANGE]: "<<pRspInfo->ErrorID<<"\t"<<gbk2utf(pRspInfo->ErrorMsg)<<endl;
    cerr<<"!-->>> fut_execute ERROR: "<<pRspInfo->ErrorID<<"\t"<<gbk2utf(pRspInfo->ErrorMsg)<<endl;
}


void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    cerr << "--->>> fut_execute: " << "OnRspOrderInsert" << endl;
    for (int i=0; i<futures_order.AllOrders.size(); i++){
        if ( futures_order.AllOrders[i]->id == atoi(pInputOrder->OrderRef) ){
            futures_order.AllOrders[i]->CTPError = pRspInfo->ErrorID;
            strcpy( futures_order.AllOrders[i]->CTPErrorString, pRspInfo->ErrorMsg);
            cerr<<(futures_order.AllOrders[i]->CTPErrorString)<<endl;
            //trade_engine.InRound=false;
            //trade_engine.Status =false;
            futures_order.AllOrders[i]->Status = 100;
        }
    }
    FileLogs->error_log<<"CTP ERROR: "<<pRspInfo->ErrorID<<"\t"<<gbk2utf(pRspInfo->ErrorMsg)<<endl;
    cerr<<"!-->>> fut_execute ERROR: "<<pRspInfo->ErrorID<<"\t"<<gbk2utf(pRspInfo->ErrorMsg)<<endl;
    IsErrorRspInfo(pRspInfo);
}




void CTraderSpi::ReqOrderAction(std::shared_ptr<FuturesOrder> TO)
{
	static bool ORDER_ACTION_SENT = false;		
	// check if the order has been sent
    //if (ORDER_ACTION_SENT)
    //	return;
	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
    /// Broker ID
    strcpy(req.BrokerID, para.p_futuresconfig->BROKER_ID.c_str());
    /// Investor ID
    strcpy(req.InvestorID, para.p_futuresconfig->INVESTOR_ID.c_str());
    /// User ID
    strcpy(req.UserID, para.p_futuresconfig->INVESTOR_ID.c_str());

	///报单操作引用
    //TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
    cerr<<"!-->>> fut_execute: canceling "<<TO->id<<endl;
    sprintf(req.OrderRef, "%012d", TO->id);
	///请求编号
	///前置编号
    req.FrontID = TO->FrontID;
	///会话编号
    req.SessionID = TO->SessionID;
	///交易所代码
    strcpy(req.ExchangeID, TO->ExchangeID.c_str());
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;

	///合约代码
    strcpy(req.InstrumentID, TO->InstrumentName.c_str());
	int iResult = TraderApi->ReqOrderAction(&req, ++iTradeRequestID);
    FileLogs->futures_order_detail <<wall_clock.printTime()
                   <<setw(15)<<req.OrderRef
                   <<setw(10)<<req.InstrumentID
                   <<setw(22)<< "cancell-initiated"<<endl;
	cerr << "--->>> Order Action Request: " << ((iResult == 0) ? "Success" : "Failed") << endl;
	ORDER_ACTION_SENT = true;
}



void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
    	cerr<<gbk2utf(pRspInfo->ErrorMsg)<<endl;
}


///报单通知
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
    cerr <<"--->>> fut_execute[Order Report]: Status="<<pOrder->OrderStatus<<"  orderSysID="<<setw(8)<<pOrder->OrderSysID<<setw(17)<<"OrdReference="<<pOrder->OrderRef<<endl;
    if (IsMyOrder(pOrder))
    {
        //if (IsTradingOrder(pOrder))
        //    ;
		//	ReqOrderAction(pOrder);
        string ss;
        for (int i=0; i<futures_order.AllOrders.size(); i++)
        {
            if (futures_order.AllOrders[i]->id == atoi(pOrder->OrderRef))
            {
                futures_order.AllOrders[i]->InitTime = pOrder->InsertTime;
                futures_order.AllOrders[i]->LastTime = pOrder->UpdateTime;
                futures_order.AllOrders[i]->OrderSysID = pOrder->OrderSysID;
                futures_order.AllOrders[i]->ExchangeID = pOrder->ExchangeID;
                futures_order.AllOrders[i]->SessionID = pOrder->SessionID;
                futures_order.AllOrders[i]->FrontID = pOrder->FrontID;
                futures_order.AllOrders[i]->TradeNumber = pOrder->VolumeTraded;
                
                switch(pOrder->OrderStatus){
                    case THOST_FTDC_OST_AllTraded:
                        ss="success";
                        //trade_engine.InRound=false;
                        //trade_engine.Status = true;
                        futures_order.AllOrders[i]->Status = 1; // success!
                        futures_order.AllOrders[i]->isActive = false;
                        break;
                    case THOST_FTDC_OST_PartTradedQueueing:
                        ss="PartTrad(in-queue)";
                        futures_order.AllOrders[i]->Status = 4; // still in queue, looks good
                        break;
                    case THOST_FTDC_OST_PartTradedNotQueueing:
                        ss="PartTrad(out-queue)";
                        futures_order.AllOrders[i]->Status = 2; // partly traded, order eliminated
                        futures_order.AllOrders[i]->isActive = false;
                        break;
                    case  THOST_FTDC_OST_NoTradeQueueing:
                        ss="UnTraded(in queue)";
                        futures_order.AllOrders[i]->Status = 5; // still in queue, heartbeating
                        break;
                    case THOST_FTDC_OST_NoTradeNotQueueing:
                        ss= "dead(out-queue)";
                        futures_order.AllOrders[i]->isActive = false;
                        futures_order.AllOrders[i]->Status = 3; // failed
                        break;
                    case THOST_FTDC_OST_Canceled:
                        ss="canceled";
                        futures_order.AllOrders[i]->Status = -2; // cancelled, waiting for replacement
                        break;
                    case THOST_FTDC_OST_Unknown:
                        ss="unkown";
                        futures_order.AllOrders[i]->Status = 50; // unkown status
                    default:
                        futures_order.AllOrders[i]->Status = 0; // which means it is failed
                        //trade_engine.InRound=false;
                        //trade_engine.Status =false;
                        break;
                }
            }
        }
        FileLogs->futures_order_detail <<wall_clock.printTime()
                       <<setw(15)<<pOrder->OrderRef
                       <<setw(10)<<pOrder->InstrumentID
                       <<setw(7)<<(pOrder->Direction==THOST_FTDC_D_Sell?"sell":"buy")
                       <<setw(7)<<(pOrder->CombOffsetFlag[0]=='0'?"open":"close")
                       <<setw(10)<< pOrder->LimitPrice
                       <<setw(7)<< pOrder->VolumeTotalOriginal
                       <<setw(7)<< pOrder->VolumeTraded
                       <<setw(8)<< (pOrder->OrderPriceType==THOST_FTDC_OPT_AnyPrice?"Market":"Limit")
                       <<setw(22)<< ss
                       << endl;

	}
}

///成交通知
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade )
{
    cerr << "--->>> fut_execute[Trade Report]: ID="<<pTrade->InstrumentID<<" Vol="<< pTrade->Volume <<" offset=" <<pTrade->OffsetFlag<< endl;

    for (int i=0; i<futures_book.InstrumentList.size();i++)
    {
        if (  futures_book.InstrumentList[i]->name == pTrade->InstrumentID )
        {
            if (pTrade->OffsetFlag == '1')
            { // position is closed

                futures_book.InstrumentList[i]->TodayFee += pTrade->Volume*pTrade->Price*300*para.p_futuresconfig-> CloseYesterdayFeeRate;
                cerr<<"--->>> fut_execute--fee incured: [cash]"<<futures_book.cash<<"-[fee]"<<pTrade->Volume*pTrade->Price*300*para.p_futuresconfig->CloseYesterdayFeeRate;
                futures_book.cash -= pTrade->Volume*pTrade->Price*300*para.p_futuresconfig->CloseYesterdayFeeRate;
                
		cerr<<"=[Cash]"<<futures_book.cash<<endl;
                if ( pTrade->Direction == THOST_FTDC_D_Buy)
                {
		    cerr<<"--->>> Trader--PL realized: [cash]"<<futures_book.cash<<"+[PL]"
			<<(futures_book.InstrumentList[i]->AV - pTrade->Price)*pTrade->Volume*300;
                    futures_book.cash += (futures_book.InstrumentList[i]->AV - pTrade->Price)*pTrade->Volume*300;
	            cerr<<"=[cash]"<<futures_book.cash<<endl;
                    futures_book.InstrumentList[i]->RealizedPL += (futures_book.InstrumentList[i]->AV - pTrade->Price)*pTrade->Volume*300;
                }
                else if (pTrade->Direction == THOST_FTDC_D_Sell)
                {
		    cerr<<"--->>> Trader--PL realized: [cash]"<<futures_book.cash<<"+[PL]"
			<<(futures_book.InstrumentList[i]->AV - pTrade->Price)*pTrade->Volume*300;
                    futures_book.cash +=  (pTrade->Price - futures_book.InstrumentList[i]->AV )*pTrade->Volume*300;
		    cerr<<"=[cash]"<<futures_book.cash<<endl;
                    futures_book.InstrumentList[i]->RealizedPL += (pTrade->Price - futures_book.InstrumentList[i]->AV )*pTrade->Volume*300;
                }
		cerr<<"--->>> Trader Ins:"<<futures_book.InstrumentList[i]->name<<" AV: "<< futures_book.InstrumentList[i]->AV<<" unchanged "<<endl;;

		 
           }
            else if  (pTrade->OffsetFlag=='0')
            {// position is opened
                futures_book.InstrumentList[i]->TodayFee += pTrade->Volume*pTrade->Price*300*para.p_futuresconfig->OpenFeeRate;
                cerr<<"--->>> Trader--fee incured: [cash]"<<futures_book.cash<<"-[fee]"<<pTrade->Volume*pTrade->Price*300*para.p_futuresconfig->CloseYesterdayFeeRate;
                futures_book.cash -= pTrade->Volume*pTrade->Price*300*para.p_futuresconfig->CloseYesterdayFeeRate;
		cerr<<"=[cash]"<<futures_book.cash<<endl;

        	cerr<<"--->>> Trader Ins:"<<futures_book.InstrumentList[i]->name<<" AV: "<< futures_book.InstrumentList[i]->AV<<" --> ";
                if ( pTrade->Direction == THOST_FTDC_D_Buy)
                {
                    futures_book.InstrumentList[i]->AV = (futures_book.InstrumentList[i]->AV*futures_book.InstrumentList[i]->Pos + pTrade->Volume*pTrade->Price)/(futures_book.InstrumentList[i]->Pos+pTrade->Volume);
                }
                else if (pTrade->Direction == THOST_FTDC_D_Sell)
                {
                    futures_book.InstrumentList[i]->AV = (-1.0*futures_book.InstrumentList[i]->AV*futures_book.InstrumentList[i]->Pos + pTrade->Volume*pTrade->Price)/(-1.0*futures_book.InstrumentList[i]->Pos+pTrade->Volume);
                }
		cerr<<futures_book.InstrumentList[i]->AV<<endl;
            }
            if( pTrade->Direction == THOST_FTDC_D_Buy)
                    futures_book.InstrumentList[i]->Pos += pTrade->Volume;
            else if ( pTrade->Direction == THOST_FTDC_D_Sell )
                    futures_book.InstrumentList[i]->Pos -= pTrade->Volume;
        }
        futures_book.InstrumentList[i]->Pos_Trading = 0;
    }
    bool fflag=false;
    for (int i=0; i<futures_order.AllOrders.size(); i++)
    {
        if (futures_order.AllOrders[i]->id == atoi(pTrade->OrderRef))
        {
            futures_order.AllOrders[i]->OrderSysID = pTrade->OrderSysID;
            futures_order.AllOrders[i]->ExchangeID = pTrade->ExchangeID;
            futures_order.AllOrders[i]->TradePrice = pTrade->Price;
            futures_order.AllOrders[i]->FinishTime = pTrade->TradeTime;
            futures_order.AllOrders[i]->Number  -=  pTrade->Volume;
            fflag=true;
        }

    }
    if (!fflag)
    {
        cerr<<"!-->>> Trade Warning: System seems to have received order information from other system"<<endl;
    }


  FileLogs->futures_report_order <<setw(15) << pTrade->OrderRef
                    <<setw(7)  << pTrade->InstrumentID
                    <<setw(7)  << (pTrade->Direction==THOST_FTDC_D_Sell?"sell":"buy")
                    <<setw(7)  << (pTrade->OffsetFlag=='0'?"open":"close")
                    <<setw(10) << pTrade->Price
                    <<setw(7)  << pTrade->Volume
                    <<setw(11) << pTrade->TradeTime
                    <<setw(15) << endl;

  FileLogs->futures_order_detail <<printDate(0)<<setw(12)<<printTime(0)
                <<setw(10)<<pTrade->InstrumentID
                <<setw(7)<<(pTrade->Direction==THOST_FTDC_D_Sell?"sell":"buy")
                <<setw(7)<<(pTrade->OffsetFlag=='0'?"open":"close")
                <<setw(10)<<pTrade->Volume
                <<setw(12)<<pTrade->Price
                <<setw(8)<<futures_book.instruments[pTrade->InstrumentID]->Pos
                <<endl;


}


void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
    FileLogs->error_log <<printTime(0)<<"-[FutureHedge Trader Connection ERROR]: Trader thread disconnected with errorcode="<<nReason<<endl;
}
		
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
	FileLogs->error_log<<printTime(0)<<"-[FutureHedge System ERROR]: "<<pRspInfo->ErrorID<<"\t"<< gbk2utf(pRspInfo->ErrorMsg)<<endl;
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
        cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << gbk2utf(pRspInfo->ErrorMsg) << endl;
	return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	// assure the returned order is my previous order
	return ((pOrder->FrontID == FRONT_ID) &&
			(pOrder->SessionID == SESSION_ID) &&
			(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}





