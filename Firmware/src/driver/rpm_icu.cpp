/*
 * esc_telem.cpp
 *
 *  Created on: Mar 12, 2021
 *      Author: jmajew
 */

#include "rpm_icu.hpp"


// should fit 24b
//static icucnt_t last_width;
static icucnt_t last_period;

//static void icuwidthcb(ICUDriver *icup)
//{
////	palSetPad(GPIOD, GPIOD_LED4);
//	last_width = icuGetWidthX(icup);
//}

static void icuperiodcb(ICUDriver *icup)
{

//	palClearPad(GPIOD, GPIOD_LED4);
	last_period = icuGetPeriodX(icup);
}

static ICUConfig icucfg =
{
	ICU_INPUT_ACTIVE_HIGH,
	1000000, /* 1MHz ICU clock frequency.   */
	NULL,
	icuperiodcb,
	NULL,
	ICU_CHANNEL_1,
	0U,
	0xFFFFFFFFU
};


void RpmDev::Init( ICUDriver* pdrv, icuchannel_t ch)
{
	mpICUDriver = pdrv;
	mChannel = ch;
}

void RpmDev::Start()
{
	icufreq_t freq = mpConfig->smplFreq;

	mpICUConfig = &icucfg;
	mpICUConfig->frequency = freq;
	mpICUConfig->channel = mChannel;

	icuStart( mpICUDriver, mpICUConfig );

	icuStartCapture( mpICUDriver );
	icuEnableNotifications( mpICUDriver );

}

void RpmDev::Stop()
{
	icuStopCapture( mpICUDriver );
	icuStop( mpICUDriver );
}

uint32_t RpmDev::GetOutput()
{
	icucnt_t count = last_period;
	return count;// / mpConfig->bladeCount;
}




