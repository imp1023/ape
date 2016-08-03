#include "GameHttpHandler.h"
#ifndef WIN32
#include <unistd.h>
#endif
#include "../common/const_def.h"
#include "../common/string-util.h"
#include "../common/distribution.h"

#include "../common/ServerConfig.h"
#include "../logic/GameSrvCfg.h"
#include "../logic/User.h"
#include "../common/SysLog.h"

#include "EventDefine.h"
#include "GameEventHandler.h"
#include "EventQueue.h"

extern ServerConfig serverConfig;

// 1~100为保留字段，其中：
// 登录为1；
// 主动注册为2；
// 接受邀请注册为3；
// 邀请他人注册是4；
// 支付消费为5。
// 留言为6；
// 留言回复为7；
// 如有其它类型的留言发表类操作，请填8。
// 用户登出为9；
// 角色登录为11；
// 创建角色为12；
// 角色退出为13；
// 角色实时在线为14。
// 支付充值为15。
// 
// 其它操作请开发者使用100以上的int型数据上报
enum Report2QQType
{
    R2Q_USER_UNKNOW             = 0,
    R2Q_USER_LOGIN				= 1,
    R2Q_REGISTER_SELF			= 2,
    R2Q_REGISTER_INVITE			= 3,
    R2Q_REGISTER_OTHER			= 4,
    R2Q_CONSUME					= 5,
    R2Q_LEAVE_MESSAGE			= 6,
    R2Q_LEAVE_MESSAGE_BACK		= 7,
    R2Q_LEAVE_OTHERS				= 8,
    R2Q_USER_LOGOUT				= 9,
    R2Q_PLAYER_TENFLAG          = 10,
    R2Q_PLAYER_LOGIN				= 11,
    R2Q_CREATE_PLAYER			= 12,
    R2Q_PLAYER_LOGOUT			= 13,
    R2Q_PLAYER_ONLINE			= 14,
    R2Q_FEE						= 15,
} ;

struct QQReport_Msg_Unit
{
    int		nKey;
    string	szWords;
};

const QQReport_Msg_Unit QQReportMsg[] =
{
    {R2Q_USER_UNKNOW,                   "unknow"},
    {R2Q_USER_LOGIN,                    "login"},
    {R2Q_REGISTER_SELF,                 "register"},
    {R2Q_REGISTER_INVITE,               "accept"},
    {R2Q_REGISTER_OTHER,                "invite"},
    {R2Q_CONSUME,                       "consume"},
    {R2Q_LEAVE_MESSAGE,                 "login"},
    {R2Q_LEAVE_MESSAGE_BACK,            "login"},
    {R2Q_LEAVE_OTHERS,                  "login"},
    {R2Q_USER_LOGOUT,                   "quit"},
    {R2Q_PLAYER_TENFLAG,                  "login"},
    {R2Q_PLAYER_LOGIN,                  "login"},
    {R2Q_CREATE_PLAYER,                 "login"},
    {R2Q_PLAYER_LOGOUT,                 "quit"},
    {R2Q_PLAYER_ONLINE,                 "online"},
    {R2Q_FEE,                           "recharge"},
};

// 支付类操作为1；
// 留言发表类为2；
// 写操作类为3；
// 读操作类为4；
// 其它为5
enum REPORT_OPTTYPE
{
    ROT_PAY             = 1,
    ROT_LEAVE_MESSAGE   = 2,
    ROT_WRITE           = 3,
    ROT_READ            = 4,
    ROT_OTHER		    = 5,
} ;

const int REDWAR_APPID		= 100616028;
const int REDWAR_VERSION	= 1;
const int CHANNEL_KONGJIAN  = 1;
const int CHANNEL_QQGAME    =10;

int ConvertUiSource(int nPlaytType)
{
    int uiSource = CHANNEL_KONGJIAN;

   // if (nPlaytType == PLAT_QQGAME)
	if(nPlaytType == PLAT_QHALL)
    {
        uiSource = CHANNEL_QQGAME;
    }

    return uiSource;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HttpRequireHandler::HttpRequireHandler(void)
: m_pEventHandler(NULL)
, m_bEnable(true)
, m_bRunning(false)
{
	logger_ = log4cxx::Logger::getLogger("DataHandler");

	for (int i=0;i<HTTPTHREADNUM;i++)
	{
		pthread_mutex_init(&m_mutexHttpMsg[i], NULL);
	}
}

HttpRequireHandler::~HttpRequireHandler(void)
{
	for (int i=0;i<HTTPTHREADNUM;i++)
	{
		for (list<HttpAskMsg*>::iterator iter = m_listMessage[i].begin(); iter != m_listMessage[i].end(); iter++)
		{
			HttpAskMsg *pMessageGet = (HttpAskMsg*)*iter;
			if (pMessageGet != NULL)
			{
				delete pMessageGet;
			}
		}
		m_listMessage[i].clear();
	}


	for (int i=0;i<HTTPTHREADNUM;i++)
	{
		pthread_mutex_destroy(&m_mutexHttpMsg[i]);
	}

}

void HttpRequireHandler::initThread()
{
	int ret = 0;
	if (!m_bRunning)
	{
		m_ltStart = time(NULL);
		m_bRunning = true;
		for (int i=0;i<HTTPTHREADNUM;i++)
		{
			m_ThredPara[i].pHandler = this;
			m_ThredPara[i].no       = i;
			ret = pthread_create(&m_threadHttpInfo[i], NULL, HttpRequireHandler::GameHttpProc, (void *)(&m_ThredPara[i]));
		}
	}
}


void* HttpRequireHandler::GameHttpProc(void* arg)
{
	HttpThred* pThreadPara = static_cast<HttpThred*>(arg);
	HttpRequireHandler* pHttpRequireHandler = pThreadPara->pHandler;
	pHttpRequireHandler->Run(pThreadPara->no);
	pthread_exit(NULL);
	return NULL;
}


void HttpRequireHandler::Run(int no)
{
	if (no<0 || no>=HTTPTHREADNUM)
		return;
	vector<HttpAskMsg*> vecMessage;

    unsigned int nSrvIP = inet_addr(serverConfig.gamedBindIp(m_pEventHandler->GetSrvID()).c_str());
	
	vecMessage.reserve(10240);
	
	while(m_bRunning)
	{
		for (size_t i = 0; i < vecMessage.size(); i++)
		{
			HttpAskMsg *pMessageGet = vecMessage.at(i);
			if (pMessageGet != NULL)
			{
				delete pMessageGet;
			}
		}
		vecMessage.clear();
		
		LockMessage(no);
		while (!m_listMessage[no].empty())
		{
			vecMessage.push_back(m_listMessage[no].front());
			m_listMessage[no].pop_front();
		}
		UnLockMessage(no);
		
		//HttpReq& rHttpReq = m_HttpReq[no];

		for (size_t i = 0; i < vecMessage.size(); i++)
		{
			HttpAskMsg *pMessageGet = vecMessage[i];
			if (pMessageGet != NULL && m_pEventHandler != NULL)
			{
				HttpAnswerMsg rAnswerMgr;
				rAnswerMgr.Clear();

				rAnswerMgr.set_szopenid(pMessageGet->szopenid());
				rAnswerMgr.set_nuid(pMessageGet->nuid());
				rAnswerMgr.set_nregion(pMessageGet->nregion());
				rAnswerMgr.set_ntype(pMessageGet->ntype());
				rAnswerMgr.set_ntypesec(pMessageGet->ntypesec());

				//几个服务器随机选择
				int nrand = rand()%serverConfig.web_num()+1;

				HttpReq rHttpReq;
                if (pMessageGet->btoqq())
                {
                    string rurl = rHttpReq.getRequireUrl(pMessageGet->szopenid(), pMessageGet->nuid(), pMessageGet->ntime(), pMessageGet->nuserip(),pMessageGet->ntypesec(), pMessageGet->nregion(), pMessageGet->nmodifyfee(), nSrvIP);

                    rHttpReq.clearHeader();

                    rHttpReq.setUrl(rurl);

#ifndef _WIN32
                    int nRet = rHttpReq.perform();
                    if (nRet != CURLE_OK)
                    {
                        LOG4CXX_INFO(logger_, "HTTP REPORTTOQQ ERROR,url:"<<rurl<<",ret:"<<nRet);
                    }

                    SYS_LOG(pMessageGet->nuid(), LT_Debug,0,0, pMessageGet->ntypesec()<<pMessageGet->szopenid()<<nRet);
                    SYS_LOG(0, LT_Debug,0,0,rurl);

#endif
                }
                else
                {
                    string rurl = rHttpReq.getRequireUrl(nrand,pMessageGet->szopenid(),pMessageGet->nuid(),pMessageGet->nregion(),pMessageGet->ntype(),pMessageGet->ntypesec(),pMessageGet->szargvs());

                    //string rurl = rHttpReq.getTestUrl(nrand);

                    rHttpReq.clearHeader();

                    rHttpReq.setUrl(rurl);
                    rHttpReq.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; rv:17.0) Gecko/20100101 Firefox/17.0");
                    rHttpReq.addHeader("Host", serverConfig.web_server_address(nrand));
                    rHttpReq.addHeader("Connection", "Keep-Alive");

                    int nRet = rHttpReq.perform();
                    if( nRet == CURLE_OK)
                    {
                        rAnswerMgr.set_szanswer(rHttpReq.getContent());

                        Event *pEvent = AddHttpRequest(pMessageGet->nuid(),rAnswerMgr);
                        if (pEvent != NULL)
                        {
                            m_pEventHandler->getEventQueue()->safePushEvent(pEvent);
                        }
                    }
                    else
                    {
                        LOG4CXX_INFO(logger_, "HTTP REQUEST ERROR,url:"<<rurl<<",ret:"<<nRet);
                    }
                }
			}
		}
		usleep(1000);
	}
}

bool	HttpRequireHandler::SafePushHttpRequest(string szplatid,int64 nuid,int nregion,int ntype,int ntypesec,string szargvs/* = */)
{
	if(!CanUse())
		return false;
	HttpAskMsg *pMessageGet = new HttpAskMsg;
	pMessageGet->set_szopenid(szplatid);
	pMessageGet->set_nuid(nuid);
	pMessageGet->set_nregion(nregion);
	pMessageGet->set_ntype(ntype);
	pMessageGet->set_ntypesec(ntypesec);
	pMessageGet->set_szargvs(szargvs);

	int nrand = rand()%HTTPTHREADNUM;
	LockMessage(nrand);
	m_listMessage[nrand].push_back(pMessageGet);
	UnLockMessage(nrand);

	return true;
}

bool HttpRequireHandler::SafePushHttpLogOnToQQ(User *pUser, int nDomain)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_READ);
    pMessageGet->set_ntypesec(R2Q_USER_LOGIN);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);

    return true;
}

bool HttpRequireHandler::SafePushHttpLogOutToQQ(User *pUser, int nDomain)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_READ);
    pMessageGet->set_ntypesec(R2Q_USER_LOGOUT);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);
    return true;
}

bool HttpRequireHandler::SafePushHttpAddCreditToQQ(User *pUser, int nDomain, int nModifyfee)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_PAY);
    pMessageGet->set_ntypesec(R2Q_FEE);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_nmodifyfee(nModifyfee * 10);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);

    return true;
}

bool HttpRequireHandler::SafePushHttpWebBuyToQQ(User *pUser, int nDomain, int nModifyfee)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_PAY);
    pMessageGet->set_ntypesec(R2Q_CONSUME);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_nmodifyfee(nModifyfee * 10);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);

    return true;
}

bool HttpRequireHandler::SafePushHttpSelfRegistToQQ(User *pUser, int nDomain)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_WRITE);
    pMessageGet->set_ntypesec(R2Q_REGISTER_SELF);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);

    return true;
}

bool HttpRequireHandler::SafePushHttpInviteToQQ(User *pUser, int nDomain)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_READ);
    pMessageGet->set_ntypesec(R2Q_REGISTER_OTHER);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);

    return true;
}

bool HttpRequireHandler::SafePushHttpAcceptToQQ(User *pUser, int nDomain)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_WRITE);
    pMessageGet->set_ntypesec(R2Q_REGISTER_INVITE);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);

    return true;
}

bool HttpRequireHandler::SafePushHttpOnLineToQQ(User *pUser, int nDomain)
{
    if(!CanUse())
        return false;
    //当处理发往腾讯的http报告时  ntype表示的是REPORT_OPTTYPE ntypesec表示Report2QQType nRegion表示的是nDomain渠道类型
    HttpAskMsg *pMessageGet = new HttpAskMsg;
    pMessageGet->set_szopenid(pUser->GetPlatformId());
    pMessageGet->set_nuid(pUser->GetUid());
    pMessageGet->set_nregion(ConvertUiSource(nDomain));
    pMessageGet->set_ntype(ROT_WRITE);
    pMessageGet->set_ntypesec(R2Q_PLAYER_ONLINE);
    pMessageGet->set_btoqq(true);
//     unsigned int nSrvIP = inet_addr(pUser->GetLastLoginIp().c_str());
//     pMessageGet->set_nuserip(nSrvIP);
    pMessageGet->set_ntime((int)time(NULL));

    int nrand = rand()%HTTPTHREADNUM;
    LockMessage(nrand);
    m_listMessage[nrand].push_back(pMessageGet);
    UnLockMessage(nrand);

    return true;
}

Event*	HttpRequireHandler::AddHttpRequest(int64 uid,HttpAnswerMsg& rAnswerMgr)
{
	if (m_pEventHandler == NULL)
	{
		return NULL;
	}
	Event *pEvent = m_pEventHandler->getEventQueue()->allocateEvent();
	if (pEvent != NULL)
	{
		pEvent->set_cmd(EVETN_G2G_GAMEHTTP_REQUEST);
		pEvent->set_state(Status_Normal_Game);
		pEvent->set_time(0);
		pEvent->set_uid(uid);

		HttpAnswerMsg* pRequest = pEvent->mutable_httpanswermsg();
		if (pRequest != NULL)
		{
			pRequest->CopyFrom(rAnswerMgr);
		}

		pEvent->set_state(Status_Normal_Back_Game);
	}
	return pEvent;
}


int HttpRequireHandler::GetHttpId(int64 uid)
{
	int hash = getUidHash(uid);
	int nId = hash % serverConfig.web_num() + 1;
	return nId;
}

bool HttpRequireHandler::CanUse()
{
	int bInited = GameSrvCfg::Instance().GameHttpEnable();
	return m_bEnable && bInited && m_pEventHandler != NULL;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const string HttpReq::szkGetUrl	=	"/data_interface.war?openid={_openid_}&uid={_uid_}&region={_region_}&type={_type_}&sectype={_typesec_}&argvs={_argvs_}";
const string HttpReq::szkOpenID	=	"{_openid_}";
const string HttpReq::szkUid	=	"{_uid_}";
const string HttpReq::szkRegion	=	"{_region_}";
const string HttpReq::szkType	=	"{_type_}";
const string HttpReq::szkTypeSec	=	"{_typesec_}";
const string HttpReq::szkArgvs	=	"{_argvs_}";

const string szkQQReportCGIUrl =    "tencentlog.com/stat/report_{_reportType_}.php?version=1&appid=100616028&svrip={_svrip_}&time={_time_}&domain={_domain_}&worldid=1&opuid={_uid_}&opopenid={_openid_}&modifyfee={_modifyfee_}";
const string szkReportType = "{_reportType_}";
const string szkReportUserIP = "{_userip_}";
const string szkReportSVRIP = "{_svrip_}";
const string szkReportTime = "{_time_}";
const string szkReportModifyFee = "{_modifyfee_}";
const string szkReportDomain = "{_domain_}";


#define _replace1(src, pat, dst) \
	do {\
	size_t i = src.find(pat); \
	if (i != -1) src.replace(i, pat.size(), dst);\
	} while(0)


static size_t write_to_string(void *data, int size, int nmemb, std::string &content)
{
	long sizes = size * nmemb;
	if(data==NULL)
		return sizes;
	std::string temp((char*)(data),sizes);
	content += temp; 
	return sizes;
}

HttpReq::HttpReq(): curl_(NULL), header_(NULL)
{
	curl_ = curl_easy_init();
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_to_string);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &content_);
	curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1L);
}

HttpReq::~HttpReq()
{
	curl_easy_cleanup(curl_);
	curl_slist_free_all(header_);
}

void HttpReq::setUrl(const string& url)
{
	curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_, CURLOPT_TIMEOUT_MS, 2000);
}

void HttpReq::addHeader(const string& key, const string& val)
{
	std::string kv = key + ": " + val;
	header_ = curl_slist_append(header_, kv.c_str());
}

void HttpReq::clearHeader()
{
	curl_slist_free_all(header_);
	header_ = NULL;
}

int HttpReq::perform()
{
	content_.clear();
	if (header_ != NULL)
	{
		curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_);
		curl_easy_setopt(curl_, CURLOPT_TIMEOUT_MS, 2000);
	}
	int nRet = 1000;
	try
	{
		nRet = curl_easy_perform(curl_);
	}
	catch (...)
	{
	}
	return nRet;
}


string HttpReq::getRequireUrl(int nrand,const string& szopenid,int64 nuid,int nRegion,int ntype,int ntypesec,const string& szargvs)
{
	string outurl = "http://";
	outurl += serverConfig.web_server_address(nrand);
	outurl += ":";
	outurl += toString(serverConfig.web_gamed_Port(nrand));
	outurl += szkGetUrl;

	_replace1(outurl, szkOpenID, szopenid);
	_replace1(outurl, szkUid, toString(nuid));
	_replace1(outurl, szkRegion, toString(nRegion));
	_replace1(outurl, szkType, toString(ntype));
	_replace1(outurl, szkTypeSec, toString(ntypesec));
	_replace1(outurl, szkArgvs, szargvs);
	return outurl;
}

string HttpReq::getRequireUrl(const string& szopenid,int64 nuid, int nTime, int nIP, int nReportType, int nDomain, int nModifyfee, unsigned int nGameIP)
{
    string outurl = "http://";
    outurl += szkQQReportCGIUrl;

    _replace1(outurl, szkOpenID, szopenid);
    _replace1(outurl, szkUid, toString(nuid));
    _replace1(outurl, szkReportDomain, toString(nDomain));
    _replace1(outurl, szkReportType, QQReportMsg[nReportType].szWords);
    //_replace1(outurl, szkReportUserIP, toString(nIP));
    _replace1(outurl, szkReportTime, toString(nTime));
    _replace1(outurl, szkReportModifyFee, toString(nModifyfee));
    _replace1(outurl, szkReportSVRIP, toString(nGameIP));

    return outurl;
}

string HttpReq::getTestUrl(int nrand)
{
	string outurl = "http://192.168.0.123/net.php";
	return outurl;
}
