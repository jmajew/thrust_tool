/*
 * adc.cpp
 *
 *  Created on: 29.03.2019
 *      Author: jmajew
 */

#include "adc.hpp"


#define ADC_SENS_ARRAY_SIZE		6


const uint16_t* const ts_cal1 = (uint16_t*)0x1fff7a2c;	// 30 °C, VDDA= 3.3 V
const uint16_t* const ts_cal2 = (uint16_t*)0x1fff7a2e;	// 110 °C, VDDA= 3.3 V

const float ts_slope = (*ts_cal2 - *ts_cal1) / ( 110 - 30 );


uint16_t adc_to_temp( uint16_t val)
{
	return (uint16_t)( ( ((float)val - (float)*ts_cal1) / ts_slope + 30.0 ) * 100.0 );
}


//void	adccallback2( ADCDriver *adcp, adcsample_t *buffer, size_t n)
//{
//	(void)adcp;
//	(void)buffer;
//	(void)n;
//
////	palSetLine( LINE_ADC_END_PULSE);
//}


//static adcsample_t 	adc_samples_buf[ADC_BUF_LENGTH*ADC_ARRAY_SIZE];
//adcresult_t	tabADCResults[ADC_ARRAY_SIZE];



static void adccallback(ADCDriver *adcp)
{
	(void)adcp;
}


// ADC conversion group.
// Channels:    IN3, IN4, IN5, IN6, IN7.
static const ADCConversionGroup adcgrpcfg =
{
	FALSE,                        	   		// NOT CIRCULAR
	ADC_SENS_ARRAY_SIZE,        					// NUMB OF CH
	adccallback,                        	// ADC CALLBACK
	NULL,				           	    	// NO ADC ERROR CALLBACK
	0,                           	     	// CR1
	ADC_CR2_SWSTART,       	   				// CR2
	ADC_SMPR1_SMP_VREF(ADC_SAMPLE_144) |
	ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_480),   // SMPR1
	ADC_SMPR2_SMP_AN0(ADC_SAMPLE_144) |
	ADC_SMPR2_SMP_AN1(ADC_SAMPLE_144) |
	ADC_SMPR2_SMP_AN4(ADC_SAMPLE_144) |
	ADC_SMPR2_SMP_AN5(ADC_SAMPLE_144) ,		// SMPR2
	0,										// HTR
	0,										// LTR
	ADC_SQR1_NUM_CH(ADC_SENS_ARRAY_SIZE), 	// SQR1

	0,     // SQR2

	ADC_SQR3_SQ1_N(ADC_CHANNEL_IN0) |		// Ch0
	ADC_SQR3_SQ2_N(ADC_CHANNEL_IN1) |		// Ch1
	ADC_SQR3_SQ3_N(ADC_CHANNEL_IN4) |		// Ch4
	ADC_SQR3_SQ4_N(ADC_CHANNEL_IN5) |		// Ch5
	ADC_SQR3_SQ5_N(ADC_CHANNEL_VREFINT) |
	ADC_SQR3_SQ6_N(ADC_CHANNEL_SENSOR)		// SQR3
	//ADC_CHANNEL_SENSOR
};

// temperature:
// CAL1 :  30C at 3.3V = 917  (address 0x1fff7a2c-7a2d
// CAL2 : 110C at 3.3V = 1194 (address 0x1fff7a2e-7a2f
// uint16_t* cal1 = (uint16_t*)0x1fff7a2c;
// uint16_t* cal2 = (uint16_t*)0x1fff7a2e;



///*
// * ADC conversion group.
// * Mode:        Continuous, 16 samples of 8 channels, SW triggered.
// * Channels:    IN11, IN12, IN11, IN12, IN11, IN12, Sensor, VRef.
// */
//static const ADCConversionGroup adcgrpcfg3 = {
//  TRUE,
//  ADC_GRP2_NUM_CHANNELS,
//  adccallback,
//  adcerrorcallback,
//  0,                        /* CR1 */
//  ADC_CR2_SWSTART,          /* CR2 */
//  ADC_SMPR1_SMP_AN12(ADC_SAMPLE_56) | ADC_SMPR1_SMP_AN11(ADC_SAMPLE_56) |
//  ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_144) | ADC_SMPR1_SMP_VREF(ADC_SAMPLE_144),
//  0,                        /* SMPR2 */
//  0,                        /* HTR */
//  0,                        /* LTR */
//  0,                        /* SQR1 */
//  ADC_SQR2_SQ8_N(ADC_CHANNEL_SENSOR) | ADC_SQR2_SQ7_N(ADC_CHANNEL_VREFINT),
//  ADC_SQR3_SQ6_N(ADC_CHANNEL_IN12)   | ADC_SQR3_SQ5_N(ADC_CHANNEL_IN11) |
//  ADC_SQR3_SQ4_N(ADC_CHANNEL_IN12)   | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN11) |
//  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN12)   | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11)
//};
//


void ADConvertDev::Init( void)
{

//	resetADCResults( tabADCResults, ADC_ARRAY_SIZE);

	adcStart( &ADCD1, NULL);
	adcSTM32EnableTSVREFE();	// enable temp and vref
}

void ADConvertDev::Stop( void)
{
	adcStop( &ADCD1);
}

void ADConvertDev::ReadData()
{
//	resetADCResults( tabADCResults, ADC_ARRAY_SIZE);

	const uint8_t ntimes = mpConfig->nADCSamples;

	for ( int iadc=0; iadc<ntimes; ++iadc )
	{
		//palClearLine(LINE_ADC_END_PULSE);

		//  sampling data for demo purpose
		adcAcquireBus( &ADCD1);
		adcConvert( &ADCD1, &adcgrpcfg, (adcsample_t*) mSamplesBuf, ADC_BUF_LENGTH);
		adcReleaseBus( &ADCD1);

		AverageCur();
//		averageADCBuffer( tabCur, samples_buf2, ADC_BUF_LENGTH, ADC_ARRAY_SIZE );
//		sumADCResults( tabADCResults, tabCur, ADC_ARRAY_SIZE);
	}
}


void ADConvertDev::AverageCur()
{
	const uint8_t nsamp = ADC_BUF_LENGTH;
	const uint8_t nch = ADC_SENS_ARRAY_SIZE;

	for ( int ich=0; ich<nch; ++ich)
	{
		uint64_t tmp =  0;
		uint64_t tmp2 =  0;

		for( int ii = 0; ii<nsamp; ii++)
		{
			tmp += mSamplesBuf[ii*nch+ich];
			tmp2 += mSamplesBuf[ii*nch+ich]*mSamplesBuf[ii*nch+ich];
		}

		mtabCurRes[ich] = StatValue12( nsamp, tmp, tmp2);
	}
}


void ADConvertDev::FetchData( Adc_Data& dataOut )
{
	dataOut.mux.lock();
	const int nch = ADC_CH_COUNT;
	for ( int ich=0; ich<nch; ++ich)
	{
		dataOut.tab[ich].Reset();		// INFO :: <-----------------------
		dataOut.tab[ich].Accumulate( mtabCurRes[ mpConfig->tabCh[ich] ] );
	}
	dataOut.mux.unlock();
}




