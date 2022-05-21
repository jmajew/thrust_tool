/*
 * data.hpp
 *
 *  Created on: 15 Feb 2021
 *      Author: jmajew
 */

#ifndef __DATA_HPP__
#define __DATA_HPP__

#include "system/sysdecl.hpp"
#include "lcd/display_ssd1306.hpp"

#include "driver/adc.hpp"
#include "driver/hx711.hpp"





//struct StGaugeData
//{
//	StatValue24	tab[STGAUGE_COUNT];
//};
//
//struct ADCData
//{
//	StatValue12	tabCh[ADC_ARRAY_SIZE];
//};


class CookedMeasureData
{
public:
	StatValue16	force;
	StatValue16	torque;
	StatValue12 voltage;
	StatValue12 current;
	StatValue12	rpm;
};



////////////////////////////////////////////////////////////////////////////////////////////////////
class Motor_Data
{
public:

	void	Reset()
	{
		mux.lock();
		output =  0;
		mux.unlock();
	}

public:
	chibi::Mutex	mux;
	uint16_t		output;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class Rpm_Data
{
public:

	void	Reset()
	{
		mux.lock();
		freq =  0;
		mux.unlock();
	}

public:
	chibi::Mutex	mux;
	uint16_t		freq;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
class EscTelem_Data
{
public:
	EscTelem_Data() : volt(0xffff), curr(0xffff), rpm(0xffff), temp(0xff)	{}

	void	Reset()
	{
		mux.lock();
		volt = curr = rpm = 0xffff;
		temp = 0xff;
		mux.unlock();
	}

public:
	chibi::Mutex	mux;
	uint16_t		volt;
	uint16_t		curr;
	uint16_t		rpm;
	uint8_t			temp;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
class DataZero
{
public:
	void	Reset()
	{
		mux.lock();

		//nSmpls = 0;

		for ( int i=0; i<STGAUGE_CH_COUNT; ++i)
			tabStGauge[i].Reset();

		for ( int i=0; i<ADC_SENS_DATA_CH_COUNT; ++i)
			tabADC[i].Reset();

		mux.unlock();
	}

public:
	chibi::Mutex	mux;
	uint16_t		nSmpls;
	StatValue24		tabStGauge[STGAUGE_CH_COUNT];
	StatValue12		tabADC[ADC_SENS_DATA_CH_COUNT];
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class Data
////////////////////////////////////////////////////////////////////////////////////////////////////
class Data
{
public:
//	void	Init();

	void	Reset()
	{
		mux.lock();
		bActive = false;
		bConnAlive = false;
		mux.unlock();

		grpMotor.Reset();
		grpStGauge.Reset();
		grpAdc.Reset();
		grpRpm.Reset();
		grpEscTelem.Reset();
	}

	void	UpdateZero();
	void	PrintOnScreen( Display* plcd);

public:
	chibi::Mutex	mux;
	bool			bActive;	// measurements are active
	bool			bConnAlive;	// connection with app is on

	Motor_Data		grpMotor;
	StGauge_Data	grpStGauge;
	Adc_Data		grpAdc;
	Rpm_Data		grpRpm;
	EscTelem_Data	grpEscTelem;

	DataZero		dataZero;
};


#endif // __DATA_HPP__
