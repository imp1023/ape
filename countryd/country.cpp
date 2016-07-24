#include "country.h"
#include <pthread.h>
#include "../event/EventQueue.h"
#include "../common/string-util.h"
#include "CountryNetHandler.h"
#include "CountryEventHandler.h"
#include "CountryDataHandler.h"
#include "CountryConfigMgr.h"
#include "../logic/GameSrvCfg.h"
#include "../common/SysLog.h"
#include "../common/coredump.h"
#include "MemCacheServerHandler.h"
#include "GuildBtlDBHandler.h"
#ifdef _WIN32
#include <WinSock2.h>
#include "../common/Logger_win.h"
#else
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#endif

ServerConfig serverConfig("server.cfg");
int G_CountrySrvD_ID =0; 
//GameConfig worldConfig

CountrySrv::CountrySrv() {
	log4cxx::PropertyConfigurator::configureAndWatch("logger.cfg");
	logger_ = log4cxx::Logger::getLogger("CountrySrv");

	CountryConfigMgr::CreateInst(G_CountrySrvD_ID)->LoadCountryConfigInfo(G_CountrySrvD_ID,true);
	eq_ = new EventQueue();
	nh_ = new CountryNetHandler(eq_,G_CountrySrvD_ID);
	dh_ = new CountryDataHandler(G_CountrySrvD_ID);
	eh_ = new CountryEventHandler(eq_, dh_, nh_, G_CountrySrvD_ID);
	GuildBtlDBHandler::Instance().SetEventHandler(eh_);
	//ÊÇ·ñÆô¶¯
	if (serverConfig.IsGameLogStart())
	{
		CSysLog::GetInstance()->SetLogInfo(false,G_CountrySrvD_ID, serverConfig.GetCountryLogDir(), serverConfig.GetCountryLogName(),
			serverConfig.GetCountryLog2SrvAddr(), serverConfig.GetCountryLog2SrvPort(), serverConfig.IsGameLogStart(),
			serverConfig.GetCountryLogStatLv(), 1,0);
	}
	
//	MemCacheServerHandler::GetInst()->SetEventHandler(eh_);
}
CountrySrv::~CountrySrv() {
	delete eq_;
	delete nh_;
	delete eh_;
	CountryConfigMgr::ReleaseInst();
}

void CountrySrv::start() {

	GuildBtlDBHandler::Instance().initThread();

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
		printf("usage: country node_id\n");
		return 1;
	}
	safe_atoi(argv[1], G_CountrySrvD_ID);
	srand((unsigned int)time(NULL));
	CountrySrv halld;
	halld.start();
	return 0;
}
