/*
 * config.cpp
 *
 *  Created on: 23.03.2019
 *      Author: jmajew
 */

#include "config.hpp"

const char* const names_MotorProtocol[] =
{
    "PWM20",
    "PWM10",
    "OneShot125",
    "OneShot42",
//    "DShot300",
//    "DShot600",
//    "DShot1200"
};

const uint8_t size_MotorProtocol = ARRAYLEN(names_MotorProtocol);


const char* const names_StGaugeRate[] =
{
    "10 smpl/s",
    "80 smpl/s"
};

const uint8_t size_StGaugeRate = ARRAYLEN(names_StGaugeRate);


const char* const names_StGaugeGain[] =
{
    "64",
    "128"
};

const uint8_t size_StGaugeGain = ARRAYLEN(names_StGaugeGain);



void CfgADConvert::Pack( SBuf& sbout )
{
	sbout.WriteU16( nADCSamples );
//	sbout.WriteU8( bIbat0Active );

	for (int i=0; i<ADC_CH_COUNT; ++i)
		sbout.WriteU8( tabCh[i] );
}

void CfgADConvert::Unpack( SBuf& sbin )
{
	nADCSamples = sbin.ReadU16();
//	bIbat0Active = sbin.ReadU8();

	for (int i=0; i<ADC_CH_COUNT; ++i)
		tabCh[i] = sbin.ReadU8();
}

void CfgStrainGauge::Pack( SBuf& sbout )
{
	sbout.WriteU8( (uint8_t)sampleRate );
	sbout.WriteU8( (uint8_t)gain );

	for (int i=0; i<STGAUGE_CH_COUNT; ++i)
		sbout.WriteU8( tabCh[i] );
}

void CfgStrainGauge::Unpack( SBuf& sbin )
{
	sampleRate = (EHX711Rate)sbin.ReadU8();
	gain = (EHX711Gain)sbin.ReadU8();

	for (int i=0; i<STGAUGE_CH_COUNT; ++i)
		tabCh[i] = sbin.ReadU8();
}

void CfgMotor::Pack( SBuf& sbout )
{
	sbout.WriteU8( (uint8_t)protocol );
	sbout.WriteU16( zeroOutput );
	sbout.WriteU16( minOutput );
	sbout.WriteU16( maxOutput );
}

void CfgMotor::Unpack( SBuf& sbin )
{
	protocol = (EMotorProtocol)sbin.ReadU8();
	zeroOutput = sbin.ReadU16();
	minOutput = sbin.ReadU16();
	maxOutput = sbin.ReadU16();
}

void CfgRpm::Pack( SBuf& sbout )
{
	sbout.WriteU16( smplFreq );
//	sbout.WriteU8( bladeCount );
}

void CfgRpm::Unpack( SBuf& sbin )
{
	smplFreq = sbin.ReadU16();
//	bladeCount = sbin.ReadU8();
}

//void CfgEscTelem::Pack( SBuf& sbout )
//{
//	sbout.WriteU8( bActive );
//	sbout.WriteU8( motorPoles );
//}
//
//void CfgEscTelem::Unpack( SBuf& sbin )
//{
//	bActive = sbin.ReadU8();
//	motorPoles = sbin.ReadU8();
//}

void CfgConvert::Pack( SBuf& sbout )
{
	sbout.WriteU16( Vref );
	sbout.WriteU16( VbatDivider );
	sbout.WriteU16( VbatCorr );
	sbout.WriteU16( VIcoeff );

	sbout.WriteU16( ts_cal1 );
	sbout.WriteU16( ts_cal2 );

	for (int i=0; i<STGAUGE_CH_COUNT; ++i)
	{
		sbout.WriteU32( (uint32_t)tabStGScale[i] );
		sbout.WriteU32( (uint32_t)tabStGZero[i] );
	}

	sbout.WriteU16( LeverArmLength );
}

void CfgConvert::Unpack( SBuf& sbin )
{
	Vref = sbin.ReadU16();
	VbatDivider = sbin.ReadU16();
	VbatCorr = sbin.ReadU16();
	VIcoeff = sbin.ReadU16();

	ts_cal1 = sbin.ReadU16();
	ts_cal2 = sbin.ReadU16();

	for (int i=0; i<STGAUGE_CH_COUNT; ++i)
	{
		tabStGScale[i] = (int32_t)sbin.ReadU32();
		tabStGZero[i] = (int32_t)sbin.ReadU32();
	}

	LeverArmLength = sbin.ReadU16();
}


