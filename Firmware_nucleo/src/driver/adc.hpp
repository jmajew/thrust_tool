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



//#define ADC_ARRAY_SIZE   		6	// vref, vbat, current, current_zero, vref_int, temp_int  // ew. accx, accy, accz
#define ADC_SENS_BUF_LENGTH      	16	// 1 ore even
#define ADC_ACCEL_BUF_LENGTH      	1	// 1 ore even


//#define ADC_ID_VREF			0
//#define ADC_ID_VBATT			1
//#define ADC_ID_IBATT			2
//#define ADC_ID_IBATTZERO		3
//#define ADC_ID_MPUTEMP		4
//
////#define ADC_ID_VSUPPLY		0
////#define ADC_ID_MPUVREF		4


////////////////////////////////////////////////////////////////////////////////////////////////////
// class Adc_Data
////////////////////////////////////////////////////////////////////////////////////////////////////
class Adc_Data
{
public:

	void	Reset()
	{
		mux.lock();
		for ( int i=0; i<ADC_CH_COUNT; ++i)
			tab[i].Reset();
		mux.unlock();
	}

public:
	chibi::Mutex	mux;
	StatValue12 	tab[ADC_CH_COUNT];
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

//	bool 	IsReady( ioline_t gpioLine);

private:
	void	AverageCur();

private:
	const CfgADConvert*	mpConfig;

	adcsample_t 	mSensSamplesBuf[ ADC_SENS_BUF_LENGTH * ADC_SENS_COUNT ];
	StatValue12		mtabSensCurRes[ADC_SENS_COUNT];

	adcsample_t 	mAccelSamplesBuf[ ADC_ACCEL_BUF_LENGTH * ADC_ACCEL_COUNT ];
	StatValue12		mAcceltabCurRes[ADC_ACCEL_COUNT];
};


uint16_t adc_to_temp( uint16_t val); // return temp * 100



#endif // __DRIVER_ADC_HPP__
