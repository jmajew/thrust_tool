/*
 * master.cpp
 *
 *  Created on: 25.03.2019
 *      Author: jmajew
 */

#include "master.hpp"
#include "config_init.hpp"
#include "system/setup.h"
#include "common.hpp"


Master::Master()
	: mMotor( &mConfig.groupMotor),
	  mHX( &mConfig.groupStrain),
	  mADC( &mConfig.groupADC),
	  mRPM( &mConfig.groupRpm),
	  mProcessor( this),
	  mPort( (BaseAsynchronousChannel*)&SD_SERIAL, &mProcessor),
	  mthSerial( &mPort ),
	  mthMeasure( &mHX, &mADC, &mRPM ),
	  mthEscTelem( (BaseAsynchronousChannel*)&SD_ESC_TELEM),
//	  mthRpm( &mRPM),
	  mrefThEscTelem(nullptr),
	  mrefThRpm(nullptr)
{
	mthMeasure.SetData( &this->mData );
	mthEscTelem.SetData( &this->mData );

//	mData.Reset();
}


void Master::Init()
{
	ConfigInit::Init( &mConfig );

	mMotor.Init( &MOTOR_PWMD, MOTOR_CH);

//	mHX.Init();
//	mHX.SetGain( HX711_GAIN_128);
//	mHX.SetRate( HX711_RATE_10SPS);

//	mADC.Init();

//	mthMeasure.Init( &mHX, &mADC );
}

void Master::Start()
{
	mthSerial.start( NORMALPRIO+1 );
	mthMeasure.start( NORMALPRIO+1 );
}

void Master::Stop()
{
	// should terminate all threads ?
}

void Master::EscTelemStart()
{
	if ( ! mrefThEscTelem.isNull() )
	{
		dbg_puts("Master::EscTelemStart : mrefThEscTelem != nullptr");
		return;
	}

	mrefThEscTelem = mthEscTelem.start( NORMALPRIO+1 );
}

void Master::EscTelemStop()
{
	if ( mrefThEscTelem.isNull() )
	{
		dbg_puts("Master::EscTelemStop : mrefThEscTelem == nullptr");
		return;
	}

	mrefThEscTelem.requestTerminate();
	dbg_puts("Master::EscTelemStop : term. req.");

	msg_t msg = mrefThEscTelem.wait();
	if ( msg == MSG_OK)
	{
		dbg_puts("Master::EscTelemStop : thRpm successfuly terminated");
		mrefThEscTelem = nullptr;
	}
}

void Master::RpmStart()
{
	evtMeasure.broadcastFlags( SIG_RPM_START);
//	if ( ! mrefThRpm.isNull() )
//	{
//		dbg_puts("Master::RpmStart : mrefThRpm != nullptr");
//		return;
//	}
//
//	mrefThRpm = mthRpm.start( NORMALPRIO+1 );
}

void Master::RpmStop()
{
	evtMeasure.broadcastFlags( SIG_RPM_STOP);
//	if ( mrefThRpm.isNull() )
//	{
//		dbg_puts("Master::RpmStop : mrefThRpm == nullptr");
//		return;
//	}
//
//	mrefThRpm.requestTerminate();
//	dbg_puts("Master::RpmStop : term. req.");
//
//	msg_t msg = mrefThRpm.wait();
//	if ( msg == MSG_OK)
//	{
//		dbg_puts("Master::RpmStop : thRpm successfuly terminated");
//		mrefThRpm = nullptr;
//	}
}


void Master::MeasStart()
{
	evtMeasure.broadcastFlags( SIG_MEAS_START);
}

void Master::MeasStop()
{
	evtMeasure.broadcastFlags( SIG_MEAS_STOP);
}

void Master::MeasCalibZeroStart( uint16_t n )
{
	this->pData()->dataZero.mux.lock();
	this->pData()->dataZero.nSmpls = n;
	this->pData()->dataZero.mux.unlock();

	evtMeasure.broadcastFlags( SIG_CALIB_ZERO_START);
}

void Master::MeasCalibZeroStop()
{
	evtMeasure.broadcastFlags( SIG_CALIB_ZERO_STOP);

	// update cur config zero
	for ( int i=0; i<STGAUGE_CH_COUNT; ++i)
	{
		this->pConfig()->groupConvert.tabStGScale[i] = this->pData()->dataZero.tabStGauge[i].Mean();
//		this->pConfig()->groupConvert.tabStGZeroDev[i] = this->pData()->dataZero.tabStGauge[i].Deviation();
	}
}


void Master::MotorArm()
{
	mMotor.Start();
	mMotor.Arm();
}

void Master::MotorDisarm()
{
	mMotor.Disarm();
	mMotor.Stop();
}

void Master::MotorStop()
{
	mMotor.SetOutput( this->pConfig()->groupMotor.zeroOutput );		//MOTOR_NULL_COMMAND
}

void Master::MotorRun( uint16_t output)
{
	mMotor.SetOutput( output);
}


void Master::ShellStart()
{
	evtShell.broadcastFlags( 0x01 );
}


void Master::ApplyStGaugeConfig()
{
	mHX.SetGain();
	mHX.SetRate();
}


