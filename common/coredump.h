#pragma once
#ifndef _WIN32
#include <execinfo.h>
#include <sys/sysinfo.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <fcntl.h>
#include <unistd.h>
#endif

void saveBackTrace(int signal);
