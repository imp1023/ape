#include "worldd.h"
#include <pthread.h>
#include "../event/EventQueue.h"
#include "../common/string-util.h"
#include "WorldNetHandler.h"
#include "WorldEventHandler.h"
#include "WorldDataHandler.h"
#include "../common/coredump.h"

#ifdef _WIN32
#include <WinSock2.h>
#endif

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>

ServerConfig serverConfig("server.cfg");
int G_WorldD_ID =0; 
//GameConfig worldConfig

World::World() {
	log4cxx::PropertyConfigurator::configureAndWatch("logger.cfg");
	logger_ = log4cxx::Logger::getLogger("World");
	eq_ = new EventQueue();
	nh_ = new WorldNetHandler(eq_,G_WorldD_ID);
	dh_ = new WorldDataHandler(G_WorldD_ID);
	eh_ = new WorldEventHandler(eq_, dh_, nh_, G_WorldD_ID);
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

	// block SIGINT to all child process:
	sigset_t bset, oset;
	sigemptyset(&bset);
	sigaddset(&bset, SIGINT);
	// equivalent to sigprocmask
	if (pthread_sigmask(SIG_BLOCK, &bset, &oset) != 0) {
		printf("set thread signal mask error!");
		return 0;
	}
#endif

	if (argn!=2) {
		printf("usage: worldd node_id\n");
		return 1;
	}
	safe_atoi(argv[1], G_WorldD_ID);

	World world;
	world.start();
	return 0;
}
