
#include "Rand.h"
#include <ctime>
#include <stdio.h>

RandGen	g_RandGen	= RandGen((unsigned)time(NULL));

int RandGen::GetRand(int nStart, int nEnd)
{
	return (int)((nEnd-nStart)*g_RandGen.RandDouble()) + nStart;
}
