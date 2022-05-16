/*
 * hx711.cpp
 *
 *  Created on: 29.03.2019
 *      Author: jmajew
 */

#include "hx711.hpp"
#include "util/waitmicro.hpp"
#include "util/util.hpp"


#define HX_SENS_ARRAY_SIZE	3

const ioline_t	tabLineDATA[] = { LINE_HX_DATA1, LINE_HX_DATA2, LINE_HX_DATA3 };


chibi::BinarySemaphore	semRdy( false);


static uint8_t maskRdy;
static uint8_t flagRdy = 0;


static void button_hx_rdy(void *arg)
{
	(void) arg;

	chSysLockFromISR();

	uint32_t id = (uint32_t)arg;

	if ( palReadLine( tabLineDATA[id]) == PAL_LOW )
	{
//		dbg_puts( "button_hx_rdy triggered\r\n");
		flagRdy |= (1 << id);

		if ( flagRdy == maskRdy )  // 1 - no of gauges
		{
			flagRdy = 0;
			semRdy.signalI();
		}
	}
	chSysUnlockFromISR();
}



HX711Dev::HX711Dev( const CfgStrainGauge* ptr)
	: mpConfig(ptr),
	  count(STGAUGE_CH_COUNT),	// FIXME :: count
//	  tabLineDATA{LINE_HX_DATA1, LINE_HX_DATA2},
	  gain(1)
{
}

void HX711Dev::EnableInt()
{
	for ( int i=0; i<count; ++i)
	{
		palEnableLineEvent( tabCurLine[i], PAL_EVENT_MODE_FALLING_EDGE);
		palSetLineCallback( tabCurLine[i], button_hx_rdy, (void*)i);
	}
}

void HX711Dev::DisableInt()
{
	for ( int i=0; i<count; ++i)
		palDisableLineEvent( tabCurLine[i]);
}


void HX711Dev::Init()
{
	maskRdy = 0;
	for ( int i=0; i<count; ++i)
	{
		tabCurLine[i] = tabLineDATA[ mpConfig->tabCh[i] ];
		maskRdy |= (1 << i);
	}

	EnableInt();

	// if DT lines are already low callback will not open semaphore
	bool bIsReady = true;
	for ( int i=0; i<count; ++i )
		if ( ! IsReady( tabCurLine[i]) )	// FIXME
			bIsReady = false;

	// trigger first shift-in
	flagRdy = maskRdy;

	if ( bIsReady )
		semRdy.reset( false);

	PowerDown();
	PowerUp();

	if ( mpConfig )
	{
		SetRate( mpConfig->sampleRate );
		SetGain( mpConfig->gain);
	}
}



void HX711Dev::SetGain( EHX711Gain g)
{
	switch (g)
	{
		case HX711_GAIN_128:	// channel A, gain factor 128
			gain = 1;
			break;

		case HX711_GAIN_64:		// channel A, gain factor 64
			gain = 3;
			break;

//		case HX711_GAIN_32:		// channel B, gain factor 32 :: not supported
//			break;
	}

	palClearLine( LINE_HX_SCK );
	gptWaitMicro( 1); // 1 us delay
	ReadDataArray();
}


void HX711Dev::SetRate( EHX711Rate rate)
{
	PowerDown();

	switch ( rate)
	{
		case HX711_RATE_10SPS:
			palClearLine( LINE_HX_RATE);
			//palClearLine( LINE_LED_YELLOW);
			break;

		case HX711_RATE_80SPS:
			palSetLine( LINE_HX_RATE);
			//palSetLine( LINE_LED_YELLOW);
			break;

		default:
			// ERROR: urecognized rate
			break;
	};

	PowerUp();
}


void HX711Dev::ReadDataArray()
{

	uint8_t i;
	uint8_t data[3*HX_SENS_ARRAY_SIZE] = { 0 };
	uint8_t filler = 0x00;

	// wait for semaphore ready
	semRdy.wait();

	gptWaitMicro( 3); // ~1 us delay

	DisableInt();

	// pulse the clock pin 24 times to read the data
	shiftInArray( LINE_HX_SCK, tabCurLine, data+2*count, count, MSBFIRST);
	shiftInArray( LINE_HX_SCK, tabCurLine, data+1*count, count, MSBFIRST);
	shiftInArray( LINE_HX_SCK, tabCurLine, data, 		 count, MSBFIRST);

//	hx_is_reading = false;
	EnableInt();

	// set the channel and the gain factor for the next reading using the clock pin
	for ( i=0; i < gain; i++)
	{
		palSetLine( LINE_HX_SCK );
		gptWaitMicro( 1); // ~1 us delay
		palClearLine( LINE_HX_SCK );
		gptWaitMicro( 1); // ~1 us delay
	}

//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 1); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 1); // ~1 us delay
//
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 2); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 2); // ~1 us delay
//
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 3); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 3); // ~1 us delay
//
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 4); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 4); // ~1 us delay
//
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 15); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 15); // ~1 us delay
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 15); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 15); // ~1 us delay
//
//
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 25); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 25); // ~1 us delay
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 25); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 25); // ~1 us delay
//
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 1000); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 1000); // ~1 us delay
//	palSetLine( LINE_HX_SCK );
//	gptWaitMicro( 1000); // ~1 us delay
//	palClearLine( LINE_HX_SCK );
//	gptWaitMicro( 1000); // ~1 us delay

	for ( i=0; i<count; ++i )
	{
		// Replicate the most significant bit to pad out a 32-bit signed integer
		if ( data[ 2*count+i ] & 0x80)
			filler = 0xFF;
		else
			filler = 0x00;

		// Construct a 32-bit signed integer
		tabData[i] = ( (uint32_t)filler << 24
					| (uint32_t)(data[2*count+i]) << 16
					| (uint32_t)(data[1*count+i]) << 8
					| (uint32_t)(data[i]) );
//		dbg_printf("%2d - %d\r\n", i, tabOut[i]);
	}

}

void HX711Dev::FetchData( StGauge_Data& dataOut )
{
	dataOut.mux.lock();
	for ( int ich=0; ich<count; ++ich)	// FIXME
	{
		dataOut.tab[ich] = tabData[ich];
	}
	dataOut.mux.unlock();
}


void HX711Dev::PowerDown( void)
{
	palClearLine( LINE_HX_SCK );
	gptWaitMicro( 1); 	// ~1 us delay
	palSetLine( LINE_HX_SCK );
	chThdSleepMilliseconds( 1);		// min. 60us
}

void HX711Dev::PowerUp( void)
{
	palClearLine( LINE_HX_SCK );
	gptWaitMicro( 1); 	// 1 us delay
}

