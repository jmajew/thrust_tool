/*
 * flash.hpp
 *
 *  Created on: 23.03.2019
 *      Author: jmajew
 */

#ifndef __FLASH_FLASH_HPP__
#define __FLASH_FLASH_HPP__

#include "system/sysdecl.hpp"


enum EFlashError
{
	EE_FLASH_OK	= 0,
	EE_FLASH_ERROR_OUT_OF_MEMORY,
	EE_FLASH_ERROR_VERIFY,
	EE_FLASH_ERROR_READING_DATA,
	EE_FLASH_ERROR_FLASHING_DATA,
	EE_FLASH_ERROR_FLASHING_ADDRESS,
	EE_FLASH_ERROR_FLASH_NOT_INITIALIZED
};



////////////////////////////////////////////////////////////////////////////////////////////////////
// class Flash
////////////////////////////////////////////////////////////////////////////////////////////////////
class Flash
{
public:
	static EFlashError EraseDataSector();
	static EFlashError WriteData( void* pcfg, uint32_t cfgsize);
	static EFlashError ReadData( void* pcfg, uint32_t cfgsize);

private:
	// TODO :: add specification/handle of the flash sector ?
};

#endif // __FLASH_FLASH_HPP__
