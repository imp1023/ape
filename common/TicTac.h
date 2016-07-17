#ifndef _TICTAC_H_
#define _TICTAC_H_
#pragma once
#include "const_def.h"
#include "Clock.h"

class TicTac 
{
public:
	void tic() 
	{
		tic_ = Clock::getCurrentSystemTime();
	}
	time_t tac() 
	{
		time_t tac = Clock::getCurrentSystemTime();
		time_t elapsed = tac - tic_;
		tic_ = tac;
		return elapsed;
	}
private:
	// the unit is milliseconds
	time_t tic_;
};

#endif

