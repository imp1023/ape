#include "Daemon.h"
#include <string.h>
#include "../common/string-util.h"
#include "../common/SysLog.h"
#include "../event/EventQueue.h"
#include "GameNetHandler.h"
#include "GameEventHandler.h"
#include "GameDataHandler.h"
#include "../logic/GameConfigMgr.h"
#include "../logic/GameServerConfig.h"
#include "GameCacheModule.h"
#include "../common/coredump.h"

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>

#ifdef _TEST
#include <gtest/gtest.h>
#endif

bool g_gameerrorexit = false;
Daemon Daemon::m_Daemon;

Daemon::Daemon()
    : nid_(0)
{

}

Daemon::~Daemon()
{
	quit();
}

void Daemon::Init(int nId)
{
	nid_ = nId;
	log4cxx::PropertyConfigurator::configureAndWatch("logger.cfg");
	eq_.reset(new EventQueue());
	nh_.reset(new GameNetHandler(eq_.get(), nId));
	dh_.reset(new GameDataHandler(nId));
	eh_.reset(new GameEventHandler(eq_.get(), dh_.get(), nh_.get(), nId));
	logger_ = log4cxx::Logger::getLogger("Daemon");
	LOG4CXX_INFO(logger_, "start gamed now !!!!");

	if (ServerConfig::Instance().IsGameLogStart())
	{
		CSysLog::GetInstance()->SetLogInfo(true, ServerConfig::Instance().gameregion(nId), ServerConfig::Instance().GetGameLogDir(), ServerConfig::Instance().GetGameLogName(),
			ServerConfig::Instance().GetGameLog2SrvAddr(), ServerConfig::Instance().GetGameLog2SrvPort(), ServerConfig::Instance().IsGameLogStart(),
			ServerConfig::Instance().GetGameLogStatLv(), ServerConfig::Instance().GetGameLogStatModul(), ServerConfig::Instance().GetGameLogStatModulVal());
	}
	GameCacheInst::instance().SetEventHandler(Daemon::Instance().GetGameEventHandler());
}

void Daemon::start()
{
    eh_.get()->start();
    nh_.get()->start();
}

void Daemon::quit()
{
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

void gameerrorexit()
{
	if (!GameServerConfig::Instance().ErrorSaveEnable() || g_gameerrorexit)
	{
		return;
	}

	g_gameerrorexit = true;
	printf("error exit before save all user data\n");
	Daemon::Instance().GetGameDataHandler()->saveAllUserData(true);
	//GuildModuleInst::instance().SaveAll();
	//CampBattleModuleInst::instance().SaveAll();
	//PlayerNameModuleInst::instance().saveAll();
	//CampModuleInst::instance().saveAll();
	//GoldIslandModuleInst::instance().SaveAll();
	//Daemon::Instance().GetGameDataHandler()->termThreads();
	printf("error exit after save all user data\n");
	fflush(stdout);
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

	pErrorExit = gameerrorexit;
#endif
    if (argn != 2)
    {
        printf("usage: gamed node_id\n");
        return 1;
    }

	srand((unsigned int)time(NULL));
    int nid;
    safe_atoi(argv[1], nid);
	Daemon::Instance().LoadConfig();
	Daemon::Instance().Init(nid);
	Daemon::Instance().start();
	printf("daemon quit before\n");
	Daemon::Instance().quit();
	Daemon::Instance().ClearConfig();
	printf("daemon quit after\n");
    return 0;
}
