/*
 * thsp_processor.cpp
 *
 *  Created on: 10.03.2019
 *      Author: jmajew
 */

#include "io/thsp_processor.hpp"
#include "io/thsp_protocol.h"
#include "flash/flash.hpp"
#include "master.hpp"

char git_commit[] = __REVISION__;
char gcc_date[] = __DATE__;
char gcc_time[] = __TIME__;


//static uint32_t data_id = 0;

int batteryNCellDetect( float vbat)
{
    int batteryCellCount = (int)( (vbat / 4.25f) + 1 );
    return std::min(batteryCellCount, 6);
}


ETHSerialResult	ThspProcessor::ProcessCommand( uint8_t command, SBuf& sbin, SBuf& sbout)
{
	(void)command;
	(void)sbin;
	(void)sbout;
	
	Data* 	pdata 	= mpMaster->pData();
	Config* pconfig	= mpMaster->pConfig();

	(void)pconfig;

    switch ( command )
	{
    case THSP_API_VERSION:
    {
    	sbout.WriteU8( THSP_PROTOCOL_VERSION);
    	sbout.WriteU8( API_VERSION_MAJOR);
    	sbout.WriteU8( API_VERSION_MINOR);
        break;
    }

    case 2:
    {
    	sbout.WriteU32( 0xaabbddee );
//    	sbout.WriteU16( 0xaabb );
    	break;
    }

	case THSP_SOFT_INFO:
	{
		sbout.WriteString( "thrust++ : ChibiOS RT " CH_KERNEL_VERSION " : HAL " CH_HAL_VERSION );
		break;
	}
	case THSP_BOARD_INFO:
	{
		sbout.WriteString( BOARD_NAME);
		break;
	}

	case THSP_BUILD_INFO:
	{
		sbout.WriteString( __REVISION__ " : " __DATE__ " - " __TIME__ );
		break;
	}

	case THSP_SHELL_START:
	{
		mpMaster->ShellStart();
		break;
	}

//------------------------------------------------------------------------------

	case THSP_GET_CH_COUNT:
	{
		sbout.WriteU8( MOTOR_CH_COUNT );
		sbout.WriteU8( STGAUGE_CH_COUNT );
		sbout.WriteU8( ADC_CH_COUNT );
		sbout.WriteU8( RPM_CH_COUNT );
		sbout.WriteU8( ESCTELEM_CH_COUNT );

		break;
	}

	case THSP_GET_CONFIG_ADC:
	{
		pconfig->groupADC.Pack(sbout);
		break;
	}

	case THSP_GET_CONFIG_STGAUGE:
	{
		pconfig->groupStrain.Pack(sbout);
		break;
	}

	case THSP_GET_CONFIG_MOTOR:
	{
		pconfig->groupMotor.Pack(sbout);
		break;
	}

	case THSP_GET_CONFIG_RPM:
	{
		pconfig->groupRpm.Pack(sbout);
		break;
	}

	case THSP_GET_CONFIG_CONVERT:
	{
		pconfig->groupConvert.Pack(sbout);
		break;
	}

	case THSP_READ_CONFIG_FROM_FLASH:
	{
		EFlashError error = Flash::ReadData( &pconfig, sizeof( Config ) );

		if ( error )
		{
			sbout.WriteU8(ETH_OUT_ERROR);
		}
		else
		{
			sbout.WriteU8(ETH_OUT_OK);
		}

		break;
	}

	case THSP_GET_DATA_ZERO:
	{
		pdata->dataZero.mux.lock();

		sbout.WriteU8( (uint8_t)STGAUGE_CH_COUNT );
		for (int i=0; i<STGAUGE_CH_COUNT; ++i)
		{
			uint32_t n 		= (uint32_t)pdata->dataZero.tabStGauge[i].Count();
			uint32_t x 		= (uint32_t)pdata->dataZero.tabStGauge[i].Mean();
			uint32_t dev 	= (uint32_t)pdata->dataZero.tabStGauge[i].Deviation();

			sbout.WriteU32( n );
			sbout.WriteU32( x );
			sbout.WriteU32( dev );
		}

		sbout.WriteU8( (uint8_t)ADC_CH_COUNT );
		for (int i=0; i<ADC_CH_COUNT; ++i)
		{
			uint16_t n 		= (uint16_t)pdata->dataZero.tabADC[i].Count();
			uint16_t x 		= (uint16_t)pdata->dataZero.tabADC[i].Mean();
			uint16_t dev 	= (uint16_t)pdata->dataZero.tabADC[i].Deviation();

			sbout.WriteU16( n );
			sbout.WriteU16( x );
			sbout.WriteU16( dev );
		}

		pdata->dataZero.mux.unlock();
		break;
	}

//------------------------------------------------------------------------------

	case THSP_SET_CONFIG_ADC:
	{
		pconfig->groupADC.Unpack(sbin);
		break;
	}

	case THSP_SET_CONFIG_STGAUGE:
	{
		pconfig->groupStrain.Unpack(sbin);
		mpMaster->ApplyStGaugeConfig();
		break;
	}

	case THSP_SET_CONFIG_MOTOR:
	{
		pconfig->groupMotor.Unpack(sbin);
		break;
	}

	case THSP_SET_CONFIG_RPM:
	{
		pconfig->groupRpm.Unpack(sbin);
		break;
	}

	case THSP_SET_CONFIG_CONVERT:
	{
		pconfig->groupConvert.Unpack(sbin);
		break;
	}

	case THSP_WRITE_CONFIG_TO_FLASH:
	{
		EFlashError error = Flash::WriteData( pconfig, sizeof( Config ) );

		if ( error )
		{
			sbout.WriteU8(ETH_OUT_ERROR);
		}
		else
		{
			sbout.WriteU8(ETH_OUT_OK);
		}

//		chThdSleepMilliseconds(1000);
//		// after flash tool should be rebooted
		break;
	}

	case THSP_SET_DATA_ZERO:
	{
		break;
	}

//------------------------------------------------------------------------------

	case THSP_MEASURE_START:
	{
		mpMaster->MeasStart();
		break;
	}

	case THSP_MEASURE_STOP:
	{
		mpMaster->MeasStop();
		break;
	}

	case THSP_CALIBRATE_ZERO_START:
	{
		uint16_t n = sbin.ReadU16();
		mpMaster->MeasCalibZeroStart(n);
		break;
	}

	case THSP_CALIBRATE_ZERO_STOP:
	{
		mpMaster->MeasCalibZeroStop();
		break;
	}

	case THSP_MOTOR_ARM:
	{
		pdata->grpMotor.mux.lock();
		pdata->grpMotor.output = pconfig->groupMotor.zeroOutput;	//MOTOR_NULL_COMMAND;
		pdata->grpMotor.mux.unlock();

		mpMaster->MotorArm();
		break;
	}

	case THSP_MOTOR_DISARM:
	{
		pdata->grpMotor.mux.lock();
		pdata->grpMotor.output = pconfig->groupMotor.zeroOutput;	//MOTOR_NULL_COMMAND;
		pdata->grpMotor.mux.unlock();

		//pdata->motorOutput = MOTOR_NULL_COMMAND; // mutex ???
		mpMaster->MotorDisarm();
		break;
	}

	case THSP_MOTOR_SET_COMMAND:
	{
		pdata->grpMotor.mux.lock();
		pdata->grpMotor.output = sbin.ReadU16();
		pdata->grpMotor.mux.unlock();

		//pdata->motorOutput = sbin.ReadU16(); // mutex ???
		mpMaster->MotorRun( pdata->grpMotor.output);
		break;
	}

	case THSP_MOTOR_STOP:
	{
		pdata->grpMotor.mux.lock();
		pdata->grpMotor.output = pconfig->groupMotor.zeroOutput;	//MOTOR_NULL_COMMAND;
		pdata->grpMotor.mux.unlock();

		//pdata->motorOutput = MOTOR_NULL_COMMAND; // mutex ???
		mpMaster->MotorRun( pdata->grpMotor.output);
		break;
	}

	case THSP_ESCTELEM_START:
	{
		mpMaster->EscTelemStart();
		break;
	}

	case THSP_ESCTELEM_STOP:
	{
		mpMaster->EscTelemStop();
		break;
	}


	case THSP_RPM_START:
	{
		mpMaster->RpmStart();
		break;
	}

	case THSP_RPM_STOP:
	{
		mpMaster->RpmStop();
		break;
	}

	case THSP_SEND_DATA:
	{
		systime_t time = chVTGetSystemTime();
//		sbout.WriteU32( data_id++ );
		sbout.WriteU32( time );

		// Motor
		pdata->grpMotor.mux.lock();
		sbout.WriteU16( pdata->grpMotor.output );
		pdata->grpMotor.mux.unlock();

		// StrainGauge
		pdata->grpStGauge.mux.lock();
//		sbout.WriteU8( (uint8_t)STGAUGE_CH_COUNT );		// TODO :: do not send
		for (int i=0; i<STGAUGE_CH_COUNT; ++i)
		{
			sbout.WriteU32( (uint32_t)pdata->grpStGauge.tab[ i ] );
		}
		pdata->grpStGauge.mux.unlock();

		// ADC
		pdata->grpAdc.mux.lock();
//		sbout.WriteU8( (uint8_t)ADC_CH_COUNT );			// TODO :: do not send
		for (int i=0; i<ADC_CH_COUNT; ++i)
		{
			sbout.WriteU16( (uint16_t)pdata->grpAdc.tab[i].Mean() );

//			sbout.WriteU16( (uint16_t)pdata->grpAdc.tab[i].n );
//			sbout.WriteU32( (uint32_t)pdata->grpAdc.tab[i].sx );
//			sbout.WriteU64( (uint64_t)pdata->grpAdc.tab[i].sx2 );
		}
		pdata->grpAdc.mux.unlock();

		// RPM
		pdata->grpRpm.mux.lock();
		sbout.WriteU16( pdata->grpRpm.freq );
		pdata->grpRpm.mux.unlock();

		// ESC Telemetry
		pdata->grpEscTelem.mux.lock();
		sbout.WriteU16( pdata->grpEscTelem.volt );
		sbout.WriteU16( pdata->grpEscTelem.curr );
		sbout.WriteU16( pdata->grpEscTelem.rpm );
		sbout.WriteU8( pdata->grpEscTelem.temp );
		pdata->grpEscTelem.mux.unlock();

		break;
	}

	case THSP_SEND_HEARTBEAT:
	{
		pdata->mux.lock();
		pdata->bConnAlive = true;
		pdata->mux.unlock();

		pdata->grpAdc.mux.lock();

		uint16_t ivbat = pdata->grpAdc.tab[ ID_CH_IBATT ].Mean();
		uint16_t ivref = pdata->grpAdc.tab[ ID_CH_VREF ].Mean();
//		uint16_t ivbat = pdata->grpAdc.tab[ADC_ID_VBATT].Mean();
//		uint16_t ivref = pdata->grpAdc.tab[ADC_ID_VREF].Mean();
		pdata->grpAdc.mux.unlock();

//		float adc_coeff = (float)pconfig->groupConvert.Vref / (float)ivref;
//		float vbat = (float)ivbat * adc_coeff * pconfig->groupConvert.VbatDivider * pconfig->groupConvert.VbatCorr;	// [V]
//
////		ivref = ivref != 0 ? ivref : 1;
////		float vbat = 2.048f * ivbat / (float)ivref;
//
////		int ncell = batteryNCellDetect( vbat);
////		uint16_t ivcell = (uint16_t)( vbat / (float)ncell *100.);
//		uint16_t ivcell = (uint16_t)( vbat *100.);

		sbout.WriteU16( ivbat );
		sbout.WriteU16( ivref );

		break;
	}

	case THSP_RESET:
	{
		NVIC_SystemReset();
		break;
	}

    default:
        return THSP_RESULT_CMD_UNKNOWN;
    }
	
	return THSP_RESULT_ACK; // ???
}


