/*
* th_serial.cpp
 *
 *  Created on: 18 May 2022
 *      Author: jmajew
 */
 
 #include "th_accel.hpp"
 
 
 #define FFT_SIZE	1024	// 256
 
float	tabInput[ 2 * FFT_SIZE ];
float	tabOutput[ FFT_SIZE ];


chibi::BinarySemaphore	semT2Read( true);

 
 static void gpt5_cb(GPTDriver *gptp)
 {
	(void)gptp;
	// palSetPad(GPIOD, GPIOD_LED5);
	// chSysLockFromISR();
	// gptStartOneShotI(&GPTD3, 1000);   /* 0.1 second pulse.*/
	// chSysUnlockFromISR();

	chSysLockFromISR();
	semT2Read.signalI();
	chSysUnlockFromISR();
}

#define GPT_CLOCK_DIV	25

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

//// FFT settings
//#define SAMPLES					512 			// 256 real party and 256 imaginary parts
//#define FFT_SIZE				SAMPLES / 2		// FFT size is always the same size as we have samples, so 256 in our case
//
//float32_t Input[SAMPLES];
//float32_t Output[FFT_SIZE];
//
//arm_cfft_radix4_instance_f32 S;	// ARM CFFT module
//float32_t maxValue;				// Max FFT value is stored here
//uint32_t maxIndex;				// Index in Output array where max value is
//
//
//void ComputeFFT()
//{
//	// Initialize the CFFT/CIFFT module, intFlag = 0, doBitReverse = 1
//	// Supported FFT Lengths are: 16, 64, 256, 1024
//	arm_cfft_radix4_init_f32(&S, FFT_SIZE, 0, 1);
//
//	// Process the data through the CFFT/CIFFT module
//	arm_cfft_radix4_f32(&S, Input);
//
//	// Process the data through the Complex Magniture Module for calculating the magnitude at each bin
//	arm_cmplx_mag_f32(Input, Output, FFT_SIZE);
//
//	// Calculates maxValue and returns corresponding value
//	arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);
//
//}


void AccelThread::Init()
{
	gptStart(&GPTD5, &gpt5_cfg);
}

void AccelThread::main()
{
	chRegSetThreadName( "Accel");

//	dbg_puts("AccelThread - main\r\n");

	ASSERT( mpADCDev != nullptr);

	gptStart(&GPTD5, &gpt5_cfg);

	mMode = EAccelMode::Read;
	gptStartContinuous( &GPTD5, GPT_CLOCK_DIV);
	
	int count = 0;

	systime_t now = 0, nowp = chVTGetSystemTime();


	while ( ! shouldTerminate() )
	{
		//dbg_puts("AccelThread inside loop\r\n");
		
		// wait for sig gpt5 callback
		
		if ( mMode == EAccelMode::Read )
		{
			// wait for semaphore
			semT2Read.wait();

//			now = chVTGetSystemTime();
//			dbg_printf("dt = %d\r\n", now - nowp);
//			nowp = now;

			// read data from adc
			mpADCDev->ReadData_Accel();
//			mpADCDev->FetchData_Accel();
			
			if  ( ++count == FFT_SIZE )
			{
				//dbg_puts("count ==\r\n");

				mMode = EAccelMode::Compute;
				gptStopTimer( &GPTD5);
				count = 0;
			}
		}
		
		if ( mMode == EAccelMode::Compute)
		{
			//dbg_puts("accel compute\r\n");

			now = chVTGetSystemTime();
			dbg_printf("accel dt = %d\r\n", now - nowp);

			sleep( TIME_MS2I( 5));

			// do some computing

			mMode = EAccelMode::Read;
			gptStartContinuous( &GPTD5, GPT_CLOCK_DIV);
			nowp = chVTGetSystemTime();
		}

//		sleep( TIME_MS2I( 100));  // HACK ::
	}

	exit( MSG_OK );
}


////////////////////////////////////////////////////////////////////////////////////////////////////

