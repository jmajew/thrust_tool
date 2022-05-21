/*
 * adc.hpp
 *
 *  Created on: 29.03.2019
 *      Author: jmajew
 */

#ifndef __DRIVER_ADC_HPP__
#define __DRIVER_ADC_HPP__

#include "system/sysdecl.hpp"
#include "config.hpp"

#include "util/stat_value.hpp"



#define ADC_SENS_BUF_LENGTH      	4	// 1 ore even (max.16)
#define ADC_ACCEL_BUF_LENGTH      	1	// 1 ore even



////////////////////////////////////////////////////////////////////////////////////////////////////
// class Adc_Data
////////////////////////////////////////////////////////////////////////////////////////////////////
class Adc_Data
{
public:

	void	Reset()
	{
		mux.lock();
		for ( int i=0; i<ADC_SENS_DATA_CH_COUNT; ++i)
			tab[i].Reset();
		mux.unlock();
	}

public:
	chibi::Mutex	mux;
	StatValue12 	tab[ADC_SENS_DATA_CH_COUNT];
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ADConvertDev
////////////////////////////////////////////////////////////////////////////////////////////////////
class ADConvertDev
{
public:
	ADConvertDev( const CfgADConvert* ptr) : mpConfig(ptr)		{}

	void	Init();
	void	Stop();			// TODO :: check this

	void	ReadData_Sensor();
	void	ReadData_Accel();


	void	FetchData_Sensor( Adc_Data& data );
	void	FetchData_Accel();

private:

private:
	const CfgADConvert*	mpConfig;

	adcsample_t 	mSensSamplesBuf[ ADC_SENS_BUF_LENGTH * ADC_SENS_CH_COUNT ];
	StatValue12		mtabSensCurRes[ADC_SENS_CH_COUNT];

	adcsample_t 	mAccelSamplesBuf[ ADC_ACCEL_BUF_LENGTH * ADC_ACCEL_COUNT ];
	adcsample_t		mAcceltabCurRes[ADC_ACCEL_COUNT];
};


uint16_t adc_to_temp( uint16_t val); // return temp * 100



#endif // __DRIVER_ADC_HPP__
