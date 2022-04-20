/*
 * th_measure.cpp
 *
 *  Created on: Feb 19, 2021
 *      Author: jmajew
 */

#include "th_measure.hpp"
#include "util/waitmicro.hpp"

extern Display lcd;

//static StGauge_Data dataStG;
//static Adc_Data dataAdc;

//#define  BUFF_SIZE 	48
//static char  sbuff[BUFF_SIZE];


void MeasureThread::main( void)
{
	setName( "MeasureThread");

	dbg_puts("MeasureThread - main\r\n");

	bool bGo = true;
	bool bCalibZero = false;
	bool bRpmOn = false;

	ASSERT( mpHX711Dev != nullptr);
	ASSERT( mpADCDev != nullptr);
	ASSERT( mpRPMDev != nullptr);

	mpHX711Dev->Init();
	mpHX711Dev->SetRate();
	//mpHX711Dev->SetRate(HX711_RATE_10SPS);	// HACK ::

	mpADCDev->Init();

	mpRPMDev->Init( &RPM_ICUD, RPM_ICU_CH);

	chibi::EventListener elMeasure;

	evtMeasure.registerMask( &elMeasure, EVENT_MASK(0) );

	while ( !shouldTerminate() )
	{
		palToggleLine( LINE_LED_YELLOW );

		eventmask_t evt = waitAnyEventTimeout( ALL_EVENTS, TIME_MS2I( 2) ); // CHECKME :: is 2ms ok for 80Hz stgauge
		if ( evt & EVENT_MASK(0) )
		{
			eventflags_t command = elMeasure.getAndClearFlags();
//				do something

			if ( command == SIG_MEAS_START )
			{
				bGo = true;
			}
			else if ( command == SIG_MEAS_STOP )
			{
				bGo = false;
			}
			else if ( command == SIG_CALIB_ZERO_START )
			{
				mpData->dataZero.Reset();
				bCalibZero = true;
			}
			else if ( command == SIG_CALIB_ZERO_STOP )
			{
				bCalibZero = false;
			}
			else if ( command == SIG_RPM_START )
			{
				mpRPMDev->Start();
				bRpmOn = true;
			}
			else if ( command == SIG_RPM_STOP )
			{
				mpRPMDev->Stop();
				bRpmOn = false;
			}
		}


		if ( bGo)
		{
			mpData->mux.lock();
			mpData->bActive = true;
			mpData->mux.unlock();

//			mpData->grpStGauge.Reset();
//			mpData->grpAdc.Reset();

			// TODO :: mpHX711Dev->SetRate(...);
			mpHX711Dev->PowerUp();
			mpHX711Dev->ReadDataArray();	// <-- wait for stg_ready
			mpHX711Dev->FetchData( mpData->grpStGauge);
			mpHX711Dev->PowerUp();

			// save systime
//			systime_t now = chVTGetSystemTime();

			mpADCDev->ReadData();
			mpADCDev->FetchData( mpData->grpAdc);

//			if ( bRpmOn)
//			{
////				palToggleLine( LINE_LED_YELLOW );
//
//				mpData->grpRpm.mux.lock();
//				mpData->grpRpm.freq = mpRPMDev->GetOutput();
//				mpData->grpRpm.mux.unlock();
//			}

//			if ( bCalibZero )
//			{
//				mpData->UpdateZero();
//			}


			//------------------------------------------------------------------
			// broadcast event send_data
			//dbg_puts("broadcast SIG_DATARDY");
			evtDataRdy.broadcastFlags( SIG_DATARDY );

			palToggleLine( LINE_DEBUG_2 );
		}
		else
		{
			mpData->mux.lock();
			mpData->bActive = false;
			mpData->mux.unlock();

			chibi::BaseThread::sleep( TIME_MS2I( 50));
		}

//			dbg_printf( "stg[%d] = %d\r\n", ++count, dataStG.tabStg[0]);
//			dbg_puts("MeasureThread - main - loop\r\n");
	}

	evtMeasure.unregister( &elMeasure );

	// 	TODO :: check if drivers are properly stopped
	mpHX711Dev->PowerDown();
	mpADCDev->Stop();

	dbg_puts("MeasureThread - STOPPED\r\n");

	exit( MSG_OK );
}

