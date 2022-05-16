/*
 * hx711.hpp
 *
 *  Created on: 29.03.2019
 *      Author: jmajew
 */

#ifndef __DRIVER_HX711_HPP__
#define __DRIVER_HX711_HPP__

#include "system/sysdecl.hpp"
#include "config.hpp"


//typedef int32_t	stgaugesample_t;



class StGauge_Data
{
public:

	void	Reset()
	{
		mux.lock();
		for ( int i=0; i<STGAUGE_CH_COUNT; ++i)
			tab[i] = 0;
		mux.unlock();
	}

public:
	chibi::Mutex	mux;
	int32_t 		tab[STGAUGE_CH_COUNT];
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class HX711Dev -> ADC 24bit
////////////////////////////////////////////////////////////////////////////////////////////////////
class HX711Dev
{
public:
	HX711Dev( const CfgStrainGauge* ptr);// : mpConfig(ptr), gain(1)		{}

	void	Init();

	void 	PowerUp();
	void 	PowerDown();

	//	stgaugesample_t	ReadData();
	void 	ReadDataArray();	// StGauge_Data& data );
	void	FetchData( StGauge_Data& data );

	// channel A, gain factor 128
	// channel A, gain factor 64
	// channel B, gain factor 32
	void 	SetGain( EHX711Gain gain);
	void 	SetRate( EHX711Rate rate);

	void 	SetGain()	{ SetGain(mpConfig->gain); }
	void 	SetRate()	{ SetRate(mpConfig->sampleRate); }

	bool 	IsReady( ioline_t gpioLine)			{ return ( palReadLine( gpioLine) == PAL_LOW ); }

private:
	void	EnableInt();
	void	DisableInt();

private:
	const CfgStrainGauge*	mpConfig;

	const int		count;							// no of channels
	ioline_t		tabCurLine[STGAUGE_CH_COUNT];	// lines connected to channels

	uint8_t 		gain;
	int32_t 		tabData[STGAUGE_CH_COUNT];		// raw output data
};



#endif // __DRIVER_HX711_HPP__
