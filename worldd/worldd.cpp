#include "worldd.h"
#include <pthread.h>
#include "../event/EventQueue.h"
#include "../common/SysLog.h"
#include "../common/string-util.h"
#include "WorldNetHandler.h"
#include "WorldEventHandler.h"
#include "WorldDataHandler.h"
#include "../common/coredump.h"
#include "../logic/WorldConfigMgr.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#else
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#endif

World World::m_World;

World::World()
: wid_(0){
}
World::~World() {
	delete eq_;
	delete nh_;
	delete eh_;
}

void World::start() {
	eh_->start();
	nh_->start();
}

void World::LoadConfig()
{
	WorldConfigMgr::GetInst()->LoadWorldConfigInfo();
}

void World::ClearConfig()
{
	WorldConfigMgr::GetInst()->ClearWorldConfigInfo();
}

void World::Init( int wid )
{
	wid_ = wid;
	log4cxx::PropertyConfigurator::configureAndWatch("logger.cfg");
	logger_ = log4cxx::Logger::getLogger("World");
	eq_ = new EventQueue();
	nh_ = new WorldNetHandler(eq_, wid);
	dh_ = new WorldDataHandler(wid);
	eh_ = new WorldEventHandler(eq_, dh_, nh_, wid);

	if (ServerConfig::Instance().IsWorldLogStart())
	{
		CSysLog::GetInstance()->SetLogInfo(true, wid, ServerConfig::Instance().GetWorldLogDir(), ServerConfig::Instance().GetWorldLogName(),
			ServerConfig::Instance().GetWorldLog2SrvAddr(), ServerConfig::Instance().GetWorldLog2SrvPort(), ServerConfig::Instance().IsWorldLogStart(),
			ServerConfig::Instance().GetWorldLogStatLv(), ServerConfig::Instance().GetWorldLogStatModul(), ServerConfig::Instance().GetWorldLogStatModulVal());
	}
}

int World::centerID()
{
	return ServerConfig::Instance().GetServerChannel()*100 + World::Instance().worldid();
}

void worlderrorexit()
{
	printf("error exit before save world data\n");
//     GlobalDbData::instance().SaveAll();
// 	if (globalArenaServerTblInst::instance().IsCenterServer(World::Instance().centerID()))
// 		GlobalArenaModuleInst::instance().SaveAll();
// 	int allworldid = globalArenaBaseDataTblInst::instance().getAllGlobalArenaWorld();
// 	if (World::Instance().worldid() == allworldid)
// 		AllGlobalArenaModuleInst::instance().SaveAll();
	printf("error exit after save world data\n");
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
	if (pthread_sigmask(SIG_BLOCK, &bset, &oset) != 0) {
		printf("set thread signal mask error!");
		return 0;
	}

	pErrorExit = worlderrorexit;
#endif

	if (argn!=2) {
		printf("usage: worldd node_id\n");
		return 1;
	}

    int wid = 0;
	safe_atoi(argv[1], wid);

    if (wid == 0)
    {
        printf("world id error\n");
        return 1;
    }

    srand((unsigned int)time(NULL));
	World::Instance().LoadConfig();	
	World::Instance().Init(wid);
	World::Instance().start();
	World::Instance().ClearConfig();
	return 0;
}
