/*
 * config.hpp
 *
 *  Created on: 23.03.2019
 *      Author: jmajew
 */

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__


#ifdef WIN32
	#include "sysdecl.h"
	#include "sbuf.hpp"
#else
	#include "system/sysdecl.hpp"
	#include "util/util.hpp"
	#include "util/sbuf.hpp"
#endif 


////////////////////////////////////////////////////////////////////////////////////////////////////
// measurement channels


// motor channels
#define MOTOR_CH_COUNT			1

// strain gauge channels
#define STGAUGE_SENS_COUNT		3	// count of active strain gauge sensors
#define STGAUGE_CH_COUNT		2	// thrust, torque

// ADC channels
#define ADC_SENS_COUNT			6	// count of active ADC sensors
#define ADC_CH_COUNT			5	// count of stored in data;
									// Vref, Vbat, Ibat, Ibat0, MPUtemp
// RPM channels
#define RPM_CH_COUNT			1

// ESC telemetry channels
#define ESCTELEM_CH_COUNT		4	// Vbat, Ibat, Rpm, ESCtemp


//// unused channels ids
//#define STG_ID_THRUST			0
//#define STG_ID_TORQUE			1
//
//#define ADC_ID_VREF				0
//#define ADC_ID_VBATT			1
//#define ADC_ID_IBATT			2
//#define ADC_ID_IBATTZERO		3
//#define ADC_ID_MPUTEMP			4
//
//#define ESCT_ID_VBATT			0
//#define ESCT_ID_IBATT			1
//#define ESCT_ID_RPM				2
//#define ESCT_ID_TEMP			3

////////////////////////////////////////////////////////////////////////////////////////////////////

// __attribute__((packed))



enum EMotorProtocol
{
	MOTPROT_PWM2000,
	MOTPROT_PWM1000,
	MOTPROT_ONESHOT125,
	MOTPROT_ONESHOT42,
//	MOTPROT_DSHOT300,
//	MOTPROT_DSHOT600,
//	MOTPROT_DSHOT1200
};

extern const char* const names_MotorProtocol[];
extern const uint8_t size_MotorProtocol;


enum EHX711Rate
{
	HX711_RATE_10SPS,
	HX711_RATE_80SPS
};

extern const char* const names_StGaugeRate[];
extern const uint8_t size_StGaugeRate;


enum EHX711Gain
{
	HX711_GAIN_64,
	HX711_GAIN_128
};

extern const char* const names_StGaugeGain[];
extern const uint8_t size_StGaugeGain;


 ////////////////////////////////////////////////////////////////////////////////////////////////////
// ADC config
struct CfgADConvert
{
public:
	CfgADConvert() :
		nADCSamples(16)
	{}

	void	Pack( SBuf& sbout );
	void	Unpack( SBuf& sbin );

public:
	uint16_t		nADCSamples;
	//uint8_t			bIbat0Active;

	// id:  0 - Vref, 1 - Vbat, 2 - Ibat, 3 - Ibat0, 4 - MPUtemp
	uint8_t			tabCh[ADC_CH_COUNT];
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// Strain Gauge config
struct CfgStrainGauge
{
public:
	CfgStrainGauge() :
		sampleRate(HX711_RATE_10SPS),
		gain(HX711_GAIN_128)
	{}

	void	Pack( SBuf& sbout );
	void	Unpack( SBuf& sbin );

public:
	EHX711Rate		sampleRate;
	EHX711Gain		gain;

	// id: 0 - thrust, 1 - torque
	uint8_t			tabCh[STGAUGE_CH_COUNT];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Motor config
struct CfgMotor
{
public:
	CfgMotor() : 
		protocol(MOTPROT_PWM1000), 
		zeroOutput(1000), 
		minOutput(1050), 
		maxOutput(2000)
	{}

	void	Pack( SBuf& sbout );
	void	Unpack( SBuf& sbin );

public:
	EMotorProtocol	protocol;
	uint16_t		zeroOutput;
	uint16_t		minOutput;
	uint16_t		maxOutput;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// RPM capture config
struct CfgRpm
{
public:
	CfgRpm() :
		smplFreq(10000)
	{}

	void	Pack( SBuf& sbout );
	void	Unpack( SBuf& sbin );

public:
	uint32_t	smplFreq;
	//uint8_t		bladeCount;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//// ESC Telemetry config
//struct CfgEscTelem
//{
//public:
//	void	Pack( SBuf& sbout );
//	void	Unpack( SBuf& sbin );
//
//public:
//	bool		bActive;
//	//uint8_t		motorPoles;
//};


////////////////////////////////////////////////////////////////////////////////////////////////////
struct CfgConvert
{
public:
	CfgConvert() :
		Vref(2048),
		VbatDivider(11000),
		VbatCorr(0),
		VIcoeff(1000),
		RawIzero(2048),
		ts_cal1(0),
		ts_cal2(0),
		LeverArmLength(100)
	{
		for (int i=0; i<STGAUGE_CH_COUNT; ++i)
		{
			tabStGScale[i] = 1;
			tabStGZero[i] = 0;
		}
	}

	void	Pack( SBuf& sbout );
	void	Unpack( SBuf& sbin );

public:
	uint16_t	Vref;			// mV
	uint16_t	VbatDivider;	// *1000
	uint16_t	VbatCorr;		// * 10000
	uint16_t	VIcoeff;		// mV/A * 100
	uint16_t	RawIzero;

	uint16_t	ts_cal1;
	uint16_t	ts_cal2;

	int32_t		tabStGScale[STGAUGE_CH_COUNT];
	int32_t		tabStGZero[STGAUGE_CH_COUNT];

	uint16_t	LeverArmLength;		// mm *100
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class Config
////////////////////////////////////////////////////////////////////////////////////////////////////
class Config
{
public:
	CfgADConvert	groupADC;
	CfgStrainGauge	groupStrain;
	CfgMotor		groupMotor;
	CfgRpm			groupRpm;
//	CfgEscTelem		groupEscTelem;

	CfgConvert		groupConvert;
};



#endif // __CONFIG_HPP__
