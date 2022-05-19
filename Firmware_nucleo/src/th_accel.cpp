/*
* th_serial.cpp
 *
 *  Created on: 18 May 2022
 *      Author: jmajew
 */
 
 #include "th_accel.hpp"
 
 
 #define FFT_INPUT_LENGTH	1024
 
float	tabInput[ 2 * FFT_INPUT_LENGTH ];
float	tabOutput[ 2 * FFT_INPUT_LENGTH ];


chibi::BinarySemaphore	semT2Read( false);

 
 static void gpt5_cb(GPTDriver *gptp) {

  (void)gptp;
  // palSetPad(GPIOD, GPIOD_LED5);
  // chSysLockFromISR();
  // gptStartOneShotI(&GPTD3, 1000);   /* 0.1 second pulse.*/
  // chSysUnlockFromISR();

	chSysLockFromISR();
	semT2Read.signalI();
	chSysUnlockFromISR();
}

 static const GPTConfig gpt5_cfg = 
 {
	100000,    // 100kHz timer clock.
	gpt5_cb,   // Timer callback.
	0,
	0
};

 
 ////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThAccel
////////////////////////////////////////////////////////////////////////////////////////////////////

 void ComputeFFT()
 {
 }


void AccelThread::Init()
{
  gptStart(&GPTD5, &gpt5_cfg);	
}

void AccelThread::main()
{
	chRegSetThreadName( "Accel");

	dbg_puts("AccelThread - main\r\n");

	mMode = EAccelMode::Read;
	gptStartContinuous( &GPTD5, 25);
	
	int count = 0;
	while ( ! shouldTerminate() )
	{
		dbg_puts("AccelThread inside loop\r\n");
		
		// wait for sig gpt5 callback
		
		if ( mMode == EAccelMode::Read )
		{
			// wait for semaphore
			semT2Read.wait();

			// read data from adc
			mpADCDev->ReadData_Accel();
			mpADCDev->FetchData_Accel();
			
			if  ( ++count == FFT_INPUT_LENGTH )
				mMode = EAccelMode::Compute;
		}
		
		if ( mMode == EAccelMode::Compute)
		{
			gptStopTimer( &GPTD5);
			// do some computing

			count = 0;
			mMode = EAccelMode::Read;
			gptStartContinuous( &GPTD5, 25);
		}

//		sleep( TIME_MS2I( 100));  // HACK ::
	}

	exit( MSG_OK );
}


////////////////////////////////////////////////////////////////////////////////////////////////////

