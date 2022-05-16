/*
 * test_flash.h
 *
 *  Created on: 22.11.2017
 *      Author: jmajewski
 */

#ifndef __TEST_FLASH_H__
#define __TEST_FLASH_H__

#include "flash_stm32.h"


// test simple scenarion: reading and writing 32 bit buffer
void testFlashRWSingle( void);

// Execute the read/write test for several scenarios.
void testFlashRWMulti( void);


#endif // __TEST_FLASH_H__
