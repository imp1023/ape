#define _CRT_SECURE_NO_WARNINGS   // make vs2008 happy
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../common/Log.h"

Log *logger;

const int Log::LEVEL_MIN = 0;
const int Log::LEVEL_DEBUG = 0;
const int Log::LEVEL_INFO = 1;
const int Log::LEVEL_WARNING = 2;
const int Log::LEVEL_MAX = 2;

Log::Log(void)
{
	level = LEVEL_DEBUG;
}

Log::~Log(void)
{
}

void Log::setLevel(int level)
{
	this->level = max(LEVEL_MIN, min(LEVEL_MAX, level));
}

int Log::getLevel()
{
	return level;
}

void Log::log(int level, const char *msg, va_list args)
{
	static const char* levelStr[] = {"DEBUG", "INFO", "WARNING"};
	if (level>=this->level)
	{
		char tm[32];
		time_t t;
		time(&t);
		char *tstr = ctime(&t);
		strncpy(tm, tstr, strlen(tstr)-1);
		tm[strlen(tstr)-1] = '\0';
		printf("[%s %s] ", tm, levelStr[level]);
		vprintf(msg, args);
		printf("\n");
	}
}

void Log::debug(const char *msg, ...)
{
	va_list vl;
	va_start(vl, msg);
	log(LEVEL_DEBUG, msg, vl);
	va_end(vl);
}

void Log::info(const char *msg, ...)
{
	va_list vl;
	va_start(vl, msg);
	log(LEVEL_INFO, msg, vl);
	va_end(vl);
}

void Log::warning(const char *msg, ...)
{
	va_list vl;
	va_start(vl, msg);
	log(LEVEL_WARNING, msg, vl);
	va_end(vl);
}
