/*
* th_serial.cpp
 *
 *  Created on: 18 May 2022
 *      Author: jmajew
 */
 
 #include "th_accel.hpp"
 
 
 #define FFT_INPUT_DAT_LENGTH	1024
 
 static void gpt5_cb(GPTDriver *gptp) {

  (void)gptp;
  // palSetPad(GPIOD, GPIOD_LED5);
  // chSysLockFromISR();
  // gptStartOneShotI(&GPTD3, 1000);   /* 0.1 second pulse.*/
  // chSysUnlockFromISR();
}

 static const GPTConfig gpt5_cfg = 
 {
	10000,    /* 10kHz timer clock.*/
	gpt5_cb,   /* Timer callback.*/
	0,
	0
};

 
 ////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThAccel
////////////////////////////////////////////////////////////////////////////////////////////////////

void AccelThread::Init()
{
  gptStart(&GPTD5, &gpt5_cfg);	
}

void AccelThread::main()
{
	chRegSetThreadName( "Accel");

	dbg_puts("AccelThread - main\r\n");

	mMode = EAccelMode::Read;
	
	int count = 0;
	while ( ! shouldTerminate() )
	{
		dbg_puts("AccelThread inside loop\r\n");
		
		// wait for sig gpt5 callback
		
		if ( mMode == EAccelMode::Read )
		{
			// read data from adc
			;
			
			if  ( ++count == FFT_INPUT_DAT_LENGTH )
				mMode = EAccelMode::Compute;
		}
		
		if ( mMode == EAccelMode::Compute)
		{
		}

		sleep( TIME_MS2I( 100));  // HACK ::
	}

	exit( MSG_OK );
}


////////////////////////////////////////////////////////////////////////////////////////////////////

