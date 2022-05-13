/*
 * config_init.cpp
 *
 *  Created on: 04.05.2021
 *      Author: jmajew
 */

#include "config_init.hpp"
#include "config_def.hpp"
#include "flash/flash.hpp"


void	ConfigInit::Init( Config* pcfg )
{
    chThdSleepMilliseconds(100);
//    EFlashError error = Flash::ReadData( pcfg, sizeof(Config) );
//	if ( error )
		SetDefault( pcfg );

	dbg_printf( "config sizeof = %d", sizeof(Config) );
}


void	ConfigInit::SetDefault( Config* pcfg )
{
	uint32_t i;
    for ( i = 0; i < sizeCliVar; i++)
    {
    	cliSetVarDefault( pcfg, &tabCliVar[i]);
    }

    pcfg->groupConvert.ts_cal1 = *pts_cal1;
    pcfg->groupConvert.ts_cal2 = *pts_cal2;

//    pcfg->groupConvert.ts_cal1 = *(uint16_t*)0x1fff7a2c;
//    pcfg->groupConvert.ts_cal2 = *(uint16_t*)0x1fff7a2e;
}

