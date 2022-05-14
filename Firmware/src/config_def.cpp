/*
 * config_def.cpp
 *
 *  Created on: 23.03.2019
 *      Author: jmajew
 */



//#include <string.h>
//
//#include "ch.h"
//#include "hal.h"
//#include "chprintf.h"

#include "config.hpp"
#include "config_def.hpp"
#include "util.hpp"

static const char* const lookupTable_OffOn[] =
{
    "OFF", "ON"
};

static const char* const lookupTable_MotorProtocol[] =
{
    "PWM2000",
    "PWM1000",
    "OneShot125",
    "OneShot42",
//    "DShot300",
//    "DShot600",
//    "DShot1200"
};

static const char* const lookupTable_TensRate[] =
{
    "10SPS",
    "80SPS"
};

static const char* const lookupTable_TensGain[] =
{
    "128",
    "64"
};


const LookupTableEntry lookupTables[] =
{
    { lookupTable_OffOn, 			sizeof(lookupTable_OffOn) / sizeof(char*) },
    { lookupTable_MotorProtocol, 	sizeof(lookupTable_MotorProtocol) / sizeof(char*) },
    { lookupTable_TensRate, 		sizeof(lookupTable_TensRate) / sizeof(char*) },
    { lookupTable_TensGain, 		sizeof(lookupTable_TensGain) / sizeof(char*) }
};

//#define CONFIG_OFFSET( pvar)		(((char*)pvar) - ((char*)&config))

const uint16_t* const pts_cal1 = (uint16_t*)0x1fff7a2c;
const uint16_t* const pts_cal2 = (uint16_t*)0x1fff7a2e;


const VarDef	tabCliVar[] =
{
	{ "adc_nsamp", 				VARTP_UINT16, 				{.valuedef = {1, 100, 6} }, 							offsetof( Config, groupADC) 	+ offsetof(CfgADConvert, nADCSamples) },
//	{ "adc_ibat0_active", 		VARTP_UINT8 | MODE_LOOKUP,	{.lookup = {TABLE_OFF_ON, false}}, 						offsetof( Config, groupADC) 	+ offsetof(CfgADConvert, bIbat0Active) },
	{ "adc_ch_vref", 			VARTP_UINT8, 				{.valuedef = {0, ADC_SENS_COUNT, 0} }, 					offsetof( Config, groupADC) 	+ offsetof(CfgADConvert, tabCh[ID_CH_VREF]) },
	{ "adc_ch_vbat", 			VARTP_UINT8, 				{.valuedef = {0, ADC_SENS_COUNT, 1} }, 					offsetof( Config, groupADC) 	+ offsetof(CfgADConvert, tabCh[ID_CH_VBATT]) },
	{ "adc_ch_ibat", 			VARTP_UINT8, 				{.valuedef = {0, ADC_SENS_COUNT, 3} }, 					offsetof( Config, groupADC) 	+ offsetof(CfgADConvert, tabCh[ID_CH_IBATT]) },
	{ "adc_ch_ibat0", 			VARTP_UINT8, 				{.valuedef = {0, ADC_SENS_COUNT, 2} }, 					offsetof( Config, groupADC) 	+ offsetof(CfgADConvert, tabCh[ID_CH_IBATTZERO]) },
	{ "adc_ch_mputemp", 		VARTP_UINT8, 				{.valuedef = {0, ADC_SENS_COUNT, 5} }, 					offsetof( Config, groupADC) 	+ offsetof(CfgADConvert, tabCh[ID_CH_MPUTEMP]) },

	{ "stgauge_rate", 			VARTP_UINT8 | MODE_LOOKUP, 	{.lookup = {TABLE_TENSOR_RATE, HX711_RATE_10SPS}},		offsetof( Config, groupStrain) 	+ offsetof(CfgStrainGauge, sampleRate) },
	{ "stgauge_gain", 			VARTP_UINT8 | MODE_LOOKUP, 	{.lookup = {TABLE_TENSOR_GAIN, HX711_GAIN_128}}, 		offsetof( Config, groupStrain) 	+ offsetof(CfgStrainGauge, gain) },
	{ "stgauge_ch_thrust", 		VARTP_UINT8, 				{.valuedef = {0, STGAUGE_SENS_COUNT, 0} }, 				offsetof( Config, groupStrain) 	+ offsetof(CfgStrainGauge, tabCh[ID_CH_THRUST]) },
	{ "stgauge_ch_torque",		VARTP_UINT8, 				{.valuedef = {0, STGAUGE_SENS_COUNT, 1} }, 				offsetof( Config, groupStrain) 	+ offsetof(CfgStrainGauge, tabCh[ID_CH_TORQUE]) },

	{ "motor_protocol", 		VARTP_UINT8 | MODE_LOOKUP, 	{.lookup = {TABLE_MOTOR_PROTOCOL, MOTPROT_PWM1000}},	offsetof( Config, groupMotor) 	+ offsetof(CfgMotor, protocol) },
//	{ "motor_protocol", 		VARTP_UINT8 | MODE_LOOKUP, 	{.lookup = {TABLE_MOTOR_PROTOCOL, MOTPROT_ONESHOT125}},	offsetof( Config, groupMotor) 	+ offsetof(CfgMotor, protocol) },
	{ "motor_cmd_zero", 		VARTP_UINT16, 				{.valuedef = {1000, 2000, 1000} }, 						offsetof( Config, groupMotor) 	+ offsetof(CfgMotor, zeroOutput) },
	{ "motor_cmd_min", 			VARTP_UINT16, 				{.valuedef = {1000, 2000, 1060} }, 						offsetof( Config, groupMotor) 	+ offsetof(CfgMotor, minOutput) },
	{ "motor_cmd_max", 			VARTP_UINT16, 				{.valuedef = {1000, 2000, 2000} }, 						offsetof( Config, groupMotor) 	+ offsetof(CfgMotor, maxOutput) },

	{ "rpm_smpl_freq", 			VARTP_UINT32, 				{.valuedef = {1000, 1000000, 100000L} }, 				offsetof( Config, groupRpm) 	+ offsetof(CfgRpm, smplFreq) },
	//{ "rpm_blade_count", 		VARTP_UINT8, 				{.valuedef = {1, 6, 2} }, 								offsetof( Config, groupRpm) 	+ offsetof(CfgRpm, bladeCount) },

//	{ "esctelem_active", 		VARTP_UINT8 | MODE_LOOKUP,	{.lookup = {TABLE_OFF_ON, false}}, 						offsetof( Config, groupEscTelem) + offsetof(CfgEscTelem, bActive) },
//	{ "motor_poles", 			VARTP_UINT8, 				{.valuedef = {1, 128, 14} }, 							offsetof( Config, groupEscTelem) + offsetof(CfgEscTelem, motorPoles) },

	{ "adc_vref", 				VARTP_UINT16, 				{.valuedef = {0, 5000, 2048} }, 						offsetof( Config, groupConvert)	+ offsetof(CfgConvert, Vref) },
	{ "adc_vbatt_divider", 		VARTP_UINT16, 				{.valuedef = {0, 65535, 11000} }, 						offsetof( Config, groupConvert)	+ offsetof(CfgConvert, VbatDivider) },
	{ "adc_vbatt_corr", 		VARTP_UINT16, 				{.valuedef = {0, 65535, 9918} }, 						offsetof( Config, groupConvert)	+ offsetof(CfgConvert, VbatCorr) },
	{ "adc_icoeff", 			VARTP_UINT16, 				{.valuedef = {0, 65535, 3670} }, 						offsetof( Config, groupConvert)	+ offsetof(CfgConvert, VIcoeff) },
	{ "adc_raw_zero", 			VARTP_UINT16, 				{.valuedef = {0, 4096, 2048} }, 						offsetof( Config, groupConvert)	+ offsetof(CfgConvert, RawIzero) },

	{ "stgauge_l", 				VARTP_UINT16, 				{.valuedef = {0, 65535, 3500} }, 						offsetof( Config, groupConvert)	+ offsetof(CfgConvert, LeverArmLength) },

	{ "stgauge_scale_thrust", 	VARTP_INT32, 				{.valuedef = {-0x1ffffff, 0x1ffffff, 1054501L} }, 		offsetof( Config, groupConvert) + offsetof(CfgConvert, tabStGScale[0]) },
	{ "stgauge_scale_torque", 	VARTP_INT32, 				{.valuedef = {-0x1ffffff, 0x1ffffff, 1740526L} }, 		offsetof( Config, groupConvert) + offsetof(CfgConvert, tabStGScale[1]) },

	{ "stgauge_zero_thrust", 	VARTP_INT32, 				{.valuedef = {-0x1ffffff, 0x1ffffff, 63300L} }, 		offsetof( Config, groupConvert) + offsetof(CfgConvert, tabStGZero[0]) },
	{ "stgauge_zero_torque", 	VARTP_INT32, 				{.valuedef = {-0x1ffffff, 0x1ffffff, 39200L} }, 		offsetof( Config, groupConvert) + offsetof(CfgConvert, tabStGZero[1]) },
};


const uint16_t sizeCliVar = ARRAYLEN( tabCliVar);




void cliVarBuildCheck( void)
{
	BUILD_ASSERT( LOOKUP_TABLE_COUNT == ARRAYLEN(lookupTables));
}



void cliSetVar( void *pcfg, const VarDef *var, const Var value)
{
    char *ptr = (char*)pcfg + var->offset;

    switch (var->varType & VALUE_TYPE_MASK)
    {
    case VARTP_UINT8:
        *(uint8_t*)ptr = value.uint8;
        break;

    case VARTP_INT8:
        *(int8_t*)ptr = value.int8;
        break;

    case VARTP_UINT16:
        *(uint16_t*)ptr = value.uint16;
        break;

    case VARTP_INT16:
        *(int16_t*)ptr = value.int16;
        break;

    case VARTP_UINT32:
        *(uint32_t*)ptr = value.uint32;
        break;

    case VARTP_INT32:
        *(int32_t*)ptr = value.int32;
        break;
    }
}


void cliSetVarDefault( void *pcfg, const VarDef *var)
{
    Var value = {.int32 = 0 };

    switch ( var->varType & VALUE_MODE_MASK)
    {
		case MODE_DIRECT:
		{
			value.int32 = var->config.valuedef.def;
			break;
		}

	    case MODE_LOOKUP:
	    {
			value.int32 = var->config.lookup.defElementId;
			break;
	    }
    }

    cliSetVar( pcfg, var, value);
}


void cliPrintValue( BaseSequentialStream *chp, const void *pcfg, const VarDef *var)
{
    int32_t value = 0;
    const char *ptr = (char*)pcfg + var->offset;

    switch ( var->varType & VALUE_TYPE_MASK)
    {
    case VARTP_UINT8:
        value = *(uint8_t *)ptr;
        break;

    case VARTP_INT8:
        value = *(int8_t *)ptr;
        break;

    case VARTP_UINT16:
        value = *(uint16_t *)ptr;
        break;

    case VARTP_INT16:
        value = *(int16_t *)ptr;
        break;

    case VARTP_UINT32:
        value = *(uint32_t *)ptr;
        break;

    case VARTP_INT32:
        value = *(int32_t *)ptr;
        break;
    }

    switch ( var->varType & VALUE_MODE_MASK)
    {
    case MODE_DIRECT:
    	chprintf(chp, "%d", value);
    	chprintf(chp, " [%d %d : %d]", var->config.valuedef.min, var->config.valuedef.max, var->config.valuedef.def);
        break;

    case MODE_LOOKUP:
    	chprintf( chp, "%s", lookupTables[var->config.lookup.tableIndex].values[value]);
        break;
    }
}



void cliPrintValueDef( BaseSequentialStream *chp, const VarDef *var)
{
    switch ( var->varType & VALUE_MODE_MASK)
    {
		case ( MODE_DIRECT):
		{
			chprintf(chp, "Allowed range: %d - %d : default %d\r\n", var->config.valuedef.min, var->config.valuedef.max, var->config.valuedef.def);
		}
		break;

		case ( MODE_LOOKUP):
		{
			const LookupTableEntry *tableEntry = &lookupTables[ var->config.lookup.tableIndex ];
			chprintf(chp, "Allowed values:");
			for ( size_t i = 0; i < tableEntry->valueCount ; ++i)
			{
				if (i > 0)
					chprintf(chp, ",");
				chprintf(chp, " %s", tableEntry->values[i]);
			}
            chprintf(chp, " : default %s\r\n", tableEntry->values[ var->config.lookup.defElementId ]);
		}
		break;
    }
}



