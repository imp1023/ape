#pragma once

#include <stdarg.h>
#include <algorithm>
using namespace std;

class Log
{
public:
	static const int LEVEL_MIN; // = 0;
	static const int LEVEL_DEBUG; // = 0;
	static const int LEVEL_INFO; // = 1;
	static const int LEVEL_WARNING; // = 2;
	static const int LEVEL_MAX; // = 2;

	Log(void);
	~Log(void);

	void log(int level, const char *msg, va_list args);
	void debug(const char *msg, ...);
	void info(const char *msg, ...);
	void warning(const char *msg, ...);

	void setLevel(int level);
	int getLevel();

private:
	int level;
};

extern Log *logger;
