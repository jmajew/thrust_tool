/*
 * master.hpp
 *
 *  Created on: 25.03.2019
 *      Author: jmajew
 */

#ifndef __MASTER_HPP__
#define __MASTER_HPP__

#include "system/sysdecl.hpp"

#include "config.hpp"
#include "data.hpp"

#include "driver/motor.hpp"
#include "driver/adc.hpp"
#include "driver/hx711.hpp"

#include "th_measure.hpp"
#include "th_serial.hpp"
#include "th_esctelem.hpp"
//#include "th_rpm.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class Master
////////////////////////////////////////////////////////////////////////////////////////////////////
class Master
{
public:
	Master();

	void	Init();
	void	Start();
	void	Stop();

	void	EscTelemStart();
	void	EscTelemStop();

	void	RpmStart();
	void	RpmStop();

	void	MotorArm();
	void	MotorDisarm();
	void	MotorStop();
	void	MotorRun( uint16_t output);

	void	MeasStart();
	void	MeasStop();
	void	MeasCalibZeroStart( uint16_t n);
	void	MeasCalibZeroStop();

	void	MeasFetchData();

	void	ShellStart();

	void	ApplyStGaugeConfig();

	Config*	pConfig()			{ return &mConfig; }
	Data*	pData()				{ return &mData; }

private:
	Config		mConfig;
	Data		mData;

	MotorDev		mMotor;
	HX711Dev 		mHX;
	ADConvertDev 	mADC;
	RpmDev			mRPM;

	ThspProcessor	mProcessor;
	ThspPort		mPort;

	SerialThread	mthSerial;
	MeasureThread	mthMeasure;
	EscTelemThread	mthEscTelem;
//	RpmThread		mthRpm;

	chibi::ThreadReference	mrefThEscTelem;
	chibi::ThreadReference	mrefThRpm;
//	MotorMaster		mMotor;
//	MeasureMaster	mMeasure;
};

#endif // __MASTER_HPP__
