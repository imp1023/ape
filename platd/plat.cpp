#include "plat.h"
#include <pthread.h>
#include "../event/EventQueue.h"
#include "../common/string-util.h"
#include "PlatNetHandler.h"
#include "PlatEventHandler.h"
#include "PlatDataHandler.h"
#include "PlatConfigMgr.h"
#include "../logic/GameSrvCfg.h"
#include "../common/SysLog.h"
#include "../common/coredump.h"
#include "FriendInfoHandler.h"
#include "../logic/FriendGiftCfg.h"
#include "MemCacheServerHandler.h"
#ifdef _WIN32
#include <WinSock2.h>
#endif
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

ServerConfig serverConfig("server.cfg");
int G_PlatSrvD_ID =0; 
//GameConfig worldConfig

PlatSrv::PlatSrv() {
	log4cxx::PropertyConfigurator::configureAndWatch("logger.cfg");
	logger_ = log4cxx::Logger::getLogger("PlatSrv");

	PlatConfigMgr::CreateInst(G_PlatSrvD_ID)->LoadPlatConfigInfo(G_PlatSrvD_ID,true);
	eq_ = new EventQueue();
	nh_ = new PlatNetHandler(eq_,G_PlatSrvD_ID);
	dh_ = new PlatDataHandler(G_PlatSrvD_ID);
	eh_ = new PlatEventHandler(eq_, dh_, nh_, G_PlatSrvD_ID);
	//ÊÇ·ñÆô¶¯
	if (serverConfig.IsPlatLogStart())
	{
		CSysLog::GetInstance()->SetLogInfo(false,G_PlatSrvD_ID, serverConfig.GetPlatLogDir(), serverConfig.GetPlatLogName(),
			serverConfig.GetPlatLog2SrvAddr(), serverConfig.GetPlatLog2SrvPort(), serverConfig.IsGameLogStart(),
			serverConfig.GetPlatLogStatLv(), 1,0);
	}
	MemCacheServerHandler::GetInst()->SetEventHandler(eh_);
}
PlatSrv::~PlatSrv() {
	delete eq_;
	delete nh_;
	delete eh_;
	PlatConfigMgr::ReleaseInst();
}

void PlatSrv::start() {
	if(GameSrvCfg::Instance().FriendInfoEnable())
	{
		FriendInfoHandler::Instance().initThread();
	}
	eh_->start();
	nh_->start();
}

int main(int argn, char *argv[]) // here is the entrance of daemon
{
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	// dump process id
#ifndef _WIN32
	pid_t pid = getpid();
	char filebuff[256];
	sprintf(filebuff, "%s%s.pid", argv[0], argv[1]);
	FILE * fp = fopen(filebuff, "w");
	fprintf(fp, "%d\n", pid);
	fclose(fp);

	signal(SIGSEGV, saveBackTrace);
	signal(SIGILL, saveBackTrace);
	signal(SIGFPE, saveBackTrace);
	signal(SIGABRT, saveBackTrace);
	signal(SIGTERM, saveBackTrace);
	signal(SIGKILL, saveBackTrace);
	signal(SIGXFSZ, saveBackTrace);

	// block SIGINT to all child process:
	sigset_t bset, oset;
	sigemptyset(&bset);
	sigaddset(&bset, SIGINT);
	// equivalent to sigprocmask
	if (pthread_sigmask(SIG_BLOCK, &bset, &oset) != 0)
	{
		printf("set thread signal mask error!");
		return 0;
	}
#endif
	if (argn!=2) {
		printf("usage: worldd node_id\n");
		return 1;
	}
	safe_atoi(argv[1], G_PlatSrvD_ID);
	srand((unsigned int)time(NULL));
	PlatSrv halld;
	halld.start();
	return 0;
}
