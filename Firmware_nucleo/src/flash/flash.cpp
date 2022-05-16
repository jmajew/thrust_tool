/*
 * flash.cpp
 *
 *  Created on: 23.03.2019
 *      Author: jmajew
 */

#include "flash.hpp"

#include "string.h"
#include "flash/flash_stm32.h"
#include "io/io.hpp"

// configured via linker script when building binaries.
extern uint8_t __config_start;
extern uint8_t __config_end;


EFlashError Flash::EraseDataSector( void)
{
	int errFlash = EE_FLASH_RETURN_SUCCESS;
	errFlash = EE_flashSectorErase( EE_flashSectorAt( (flashaddr_t)&__config_start) );	// clear flash sector/page

//	if ( errFlash )
//		dbg_printf( "Error erasing flash: %d\r\n", errFlash );

	if ( errFlash != EE_FLASH_RETURN_SUCCESS )
	{
		dbg_printf( "Error erasing flash sector: %d\r\n", errFlash );
		return EE_FLASH_ERROR_FLASHING_DATA;
	}

	return EE_FLASH_OK;
}


EFlashError Flash::WriteData( void* pcfg, uint32_t cfgsize)
{
	cfgsize = 2 * ((cfgsize + 1) / 2);		//align to halfword

	uint8_t* ptrb = &__config_start;
	uint8_t* ptre = &__config_end;
	uint32_t memsize = ptre - ptrb;

	uint8_t* ptr = ptrb;

	uint32_t nsize = cfgsize + sizeof(uint32_t);
	bool bReady = false;

	int errFlash = EE_FLASH_RETURN_SUCCESS;

	if ( nsize > memsize )
	{
		dbg_puts( "error: not enough memory for config\n\n");
		return EE_FLASH_ERROR_OUT_OF_MEMORY;
	}

	do
	{
		// out of space in flash
		if ( ptr+nsize > ptre )
		{
			EE_flashSectorErase( EE_flashSectorAt( (flashaddr_t)ptrb) );	// clear flash sector/page
			ptr = ptrb;
		}

		// if address is blank then write
		if ( *((uint32_t*)ptr) == 0xffffffff )
		{
			errFlash = EE_flashWrite( (flashaddr_t)( ptr + sizeof(uint32_t) ), (char*)pcfg, cfgsize);

			if ( errFlash != EE_FLASH_RETURN_SUCCESS )
				return EE_FLASH_ERROR_FLASHING_DATA;

			bReady = true;
		}
		else
		{
			ptr = (uint8_t*) *((uint32_t*)ptr);
		}
	}
	while ( ! bReady);

	uint8_t* pwhere = (uint8_t*)ptr;
	uint32_t address = (uint32_t)(ptr+nsize);

	errFlash = EE_flashWrite( (flashaddr_t)pwhere, (char*)&address, sizeof(uint32_t) );

	if ( errFlash != EE_FLASH_RETURN_SUCCESS )
		return EE_FLASH_ERROR_FLASHING_ADDRESS;

	return EE_FLASH_OK;
}


EFlashError Flash::ReadData( void* pcfg, uint32_t cfgsize)
{
	uint8_t* ptrb = &__config_start;
	uint8_t* ptre = &__config_end;
	//uint32_t memsize = ptre - ptrb;

	uint8_t* ptr = ptrb;

	bool bReady = false;

	int errFlash = EE_FLASH_RETURN_SUCCESS;

	EE_flashWaitWhileBusy();

	do
	{
		if ( ptr > ptre )
		{
			return EE_FLASH_ERROR_OUT_OF_MEMORY;
		}

		if ( *((uint32_t*)ptr) == 0xffffffff )
		{
			uint8_t* p = ptr - cfgsize;
			if ( p < ptrb )
			{
				return EE_FLASH_ERROR_FLASH_NOT_INITIALIZED;
			}

			errFlash = EE_flashRead( (flashaddr_t)p, (char*)pcfg, cfgsize);

			if ( errFlash != EE_FLASH_RETURN_SUCCESS )
				return EE_FLASH_ERROR_READING_DATA;

			bReady = true;
		}
		else
		{
			ptr = (uint8_t*)*((uint32_t*)ptr);
		}
	}
	while ( ! bReady);

	return EE_FLASH_OK;
}


