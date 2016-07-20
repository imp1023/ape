#include "Daemon.h"
#include <string.h>
#include "../common/string-util.h"
#include "../common/SysLog.h"
#include "../event/EventQueue.h"
#include "GameNetHandler.h"
#include "GameEventHandler.h"
#include "GameDataHandler.h"
#include "../logic/GameConfigMgr.h"
#include "../logic/GameSrvCfg.h"
#include "../common/coredump.h"
#include "../gamed/MemCacheServerHandler.h"
#include "GameHttpHandler.h"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

ServerConfig serverConfig("server.cfg");

#include <string>
using namespace std;

Daemon::Daemon(int nid)
{
	string logger_cfg = "logger.cfg";
	log4cxx::PropertyConfigurator::configureAndWatch(logger_cfg);
	eq	= new EventQueue();
	nh	= new GameNetHandler(eq, nid);
	dh	= new GameDataHandler(nid);
	eh	= new GameEventHandler(eq, dh, nh, nid);
	dh->SetEventHandler(eh);

	logger_ = log4cxx::Logger::getLogger("Daemon");
	//FriendInfoHandler::Instance().SetEventHandler(eh);
	//StarInfoHandler::Instance().SetEventHandler(eh);
	HttpRequireHandler::Instance().SetEventHandler(eh);
	//UserLiteInfoHandler::Instance().SetEventHandler(eh);
	//RecordDBHandler::Instance().SetEventHandler(eh);

	//ÊÇ·ñÆô¶¯
	if (serverConfig.IsGameLogStart())
	{
		CSysLog::GetInstance()->SetLogInfo(true, nid, serverConfig.GetGameLogDir(), serverConfig.GetGameLogName(),
			serverConfig.GetGameLog2SrvAddr(), serverConfig.GetGameLog2SrvPort(), serverConfig.IsGameLogStart(),
			serverConfig.GetGameLogStatLv(), serverConfig.GetGameLogStatModul(), serverConfig.GetGameLogStatModulVal());
	}

	MemCacheServerHandler::GetInst()->SetEventHandler(eh);
}

Daemon::~Daemon()
{
	//	quit();
}

void Daemon::start()
{
//  	if(GameSrvCfg::Instance().FriendInfoEnable())
//  	{
//  		FriendInfoHandler::Instance().initThread();
//  	}
// 	if(GameSrvCfg::Instance().StarInfoEnable())
// 	{
// 		StarInfoHandler::Instance().initThread();
// 	}
// 	UserLiteInfoHandler::Instance().initThread();
	if(GameSrvCfg::Instance().GameHttpEnable())
	{
		HttpRequireHandler::Instance().initThread();
	}
/*	RecordDBHandler::Instance().initThread();*/

	eh->start();
	nh->start();
}

void Daemon::quit()
{
	eh->quit();
	if (nh != NULL)
	{
		delete nh;
		nh = NULL;
	}

	if (dh != NULL)
	{
		delete dh;
		dh = NULL;
	}

	if (eh != NULL)
	{
		delete eh;
		eh = NULL;
	}

	if (eq != NULL)
	{
		delete eq;
		eq = NULL;
	}

	//CSysLog::GetInstance()->Quit();

}

void Daemon::LoadConfig()
{
	GameConfigMgr::CreateInst()->LoadGameConfigInfo();
}

void Daemon::ClearConfig()
{
	GameConfigMgr::GetInst()->ClearGameConfigInfo();
	GameConfigMgr::ReleaseInst();
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
	if (argn != 2)
	{
		printf("usage: gamed node_id\n");
		return 1;
	}
	srand((unsigned int)time(NULL));
	int nid;
	safe_atoi(argv[1], nid);
	Daemon daemon(nid);
	daemon.LoadConfig();
	daemon.start();
	printf("daemon quit before\n");
	//daemon.quit();
	daemon.ClearConfig();
	printf("daemon quit after\n");
	return 0;
}
