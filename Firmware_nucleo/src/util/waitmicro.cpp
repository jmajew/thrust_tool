/*
 * waitmicro.cpp
 *
 *  Created on: Mar 11, 2019
 *      Author: jmajew
 */

#include "util/waitmicro.hpp"
#include "system/setup.h"


// TODO :: check real clock freq.
static const GPTConfig gptcfg =
{
	3000000, 	// 10 MHz timer clock.
	NULL, 		// No callback
	0, 0
};

void gptInitTimer( void)
{
	gptStart( &TIMD_WAIT, &gptcfg);
	gptPolledDelay( &TIMD_WAIT, 10); // 10 us delay
}


// TODO :: recheck this
// tuned for -O2
//#define A_CONST 10512
//#define B_CONST	9190

#define A_CONST 1000
#define B_CONST	0

void gptWaitMicro( uint16_t time)
{
//	uint32_t t =  (uint32_t)time * A_CONST - B_CONST;
//	t = t / 2000;
	uint32_t t = time * 2; //2;
	gptPolledDelay( &TIMD_WAIT, (uint16_t)t);
}

