
#ifndef __FLASH_H__
#define __FLASH_H__

//#include <stdint.h>

#include "ch.h"
#include "hal.h"

/**
 * @brief Number of sectors in the flash memory.
 */
#if !defined(EE_FLASH_SECTOR_COUNT) || defined(__DOXYGEN__)
#define EE_FLASH_SECTOR_COUNT 12
#endif

/* Error codes */

/** @brief Flash operation successful */
#define EE_FLASH_RETURN_SUCCESS CH_SUCCESS

/** @brief Flash operation error because of denied access, corrupted memory.*/
#define EE_FLASH_RETURN_NO_PERMISSION 	-1

/** @brief Flash operation error because of bad flash, corrupted memory */
#define EE_FLASH_RETURN_BAD_FLASH 		-11
#define EE_FLASH_RETURN_BAD_NSECTOR 	-12

#define EE_FLASH_RETURN_WAIT_ERROR 		-13


#define 	CH_SUCCESS   FALSE
#define 	CH_FAILED    TRUE


#ifdef __cplusplus
extern "C" {
#endif

// Warning, flashdata_t must be unsigned!!!
#if defined(STM32F4XX) || defined(STM32F7XX)

// Maximum program/erase parallelism
//
// FLASH_CR_PSIZE_MASK is the mask to configure the parallelism value.
// FLASH_CR_PSIZE_VALUE is the parallelism value suitable for the voltage range.
//
// PSIZE(1:0) is defined as:
// 00 to program 8 bits per step
// 01 to program 16 bits per step
// 10 to program 32 bits per step
// 11 to program 64 bits per step

	#define  SECTOR_MASK   0xFFFFFF07U

	#define FLASH_CR_PSIZE_MASK         FLASH_CR_PSIZE_0 | FLASH_CR_PSIZE_1

	#if ((STM32_VDD >= 270) && (STM32_VDD <= 360)) || defined(__DOXYGEN__)
	#define FLASH_CR_PSIZE_VALUE        FLASH_CR_PSIZE_1
	typedef uint32_t flashdata_t;

	#elif (STM32_VDD >= 240) && (STM32_VDD < 270)
	#define FLASH_CR_PSIZE_VALUE        FLASH_CR_PSIZE_0
	typedef uint16_t flashdata_t;

	#elif (STM32_VDD >= 210) && (STM32_VDD < 240)
	#define FLASH_CR_PSIZE_VALUE        FLASH_CR_PSIZE_0
	typedef uint16_t flashdata_t;

	#elif (STM32_VDD >= 180) && (STM32_VDD < 210)
	#define FLASH_CR_PSIZE_VALUE        ((uint32_t)0x00000000)
	typedef uint8_t flashdata_t;

	#else
	#error "invalid VDD voltage specified"
	#endif

#elif defined(STM32F1XX)

	typedef uint16_t flashdata_t;

#endif




// Wait for the flash operation to finish.
//#define flashWaitWhileBusy() { while (FLASH->SR & FLASH_SR_BSY) { asm (  "nop" );} }
int EE_flashWaitWhileBusy( void);


// Address in the flash memory
typedef uintptr_t flashaddr_t;

// Index of a sector
typedef uint32_t flashsector_t;

// Get the size of sector.
// return sector size in bytes.
size_t EE_flashSectorSize( flashsector_t sector);

// Get the beginning address of sector.
// param Sector to retrieve the beginning address of.
// return First address (inclusive) of  sector.
flashaddr_t EE_flashSectorBegin( flashsector_t sector);

// Get the end address of @p sector.
// param sector Sector to retrieve the end address of.
// return End address (exclusive) of @p sector (i.e. beginning address of the next sector).
 flashaddr_t EE_flashSectorEnd( flashsector_t sector);

// Get the sector containing @p address.
// warning @p address must be in the flash addresses range.
// param address Address to be searched for.
// return Sector containing @p address.
 flashsector_t EE_flashSectorAt( flashaddr_t address);

// Erase the flash @p sector.
// details The sector is checked for errors after erase.
// note The sector is deleted regardless of its current state.
//
// param sector Sector which is going to be erased.
// return EE_FLASH_RETURN_SUCCESS         No error erasing the sector.
// return EE_FLASH_RETURN_BAD_FLASH       Flash cell error.
// return EE_FLASH_RETURN_NO_PERMISSION   Access denied.
 int EE_flashSectorErase( flashsector_t sector);

 // Erase the sectors containing the span of @p size bytes starting at @p address.
 //
 // warning If @p address doesn't match the beginning of a sector, the
 // data contained between the beginning of the sector and @p address will
 // be erased too. The same applies for data contained at @p address + @p size
 // up to the end of the sector.
 //
 // param address Starting address of the span in flash memory.
 // param size Size of the span in bytes.
 // return EE_FLASH_RETURN_SUCCESS         No error erasing the flash memory.
 // return EE_FLASH_RETURN_BAD_FLASH       Flash cell error.
 // return EE_FLASH_RETURN_NO_PERMISSION   Access denied.
  int EE_flashErase( flashaddr_t address, size_t size);

 // Check if the @p size bytes of flash memory starting at @p address are erased.
 // note If the memory is erased, one can write data into it safely.
 // param address First address in flash memory to be checked.
 // param size Size of the memory space to be checked in bytes.
 // return TRUE Memory is already erased.
 // return FALSE Memory is not erased.
 bool EE_flashIsErased( flashaddr_t address, size_t size);

 // Check if the data in @p buffer are identical to the one in flash memory.
 // param address First address in flash memory to be checked.
 // param buffer Buffer containing the data to compare.
 // param size Size of @p buffer in bytes.
 // return TRUE if the flash memory and the buffer contain identical data.
 // return FALSE if the flash memory and the buffer don't contain identical data.
 bool EE_flashCompare( flashaddr_t address, const char* buffer, size_t size);

 // Copy data from the flash memory to a @p buffer.
 // warning The @p buffer must be at least @p size bytes long.
 // param address First address of the flash memory to be copied.
 // param buffer Buffer to copy to.
 // param size Size of the data to be copied in bytes.
 // return EE_FLASH_RETURN_SUCCESS if successfully copied.
  int EE_flashRead( flashaddr_t address, char* buffer, size_t size);


 // Copy data from a @p buffer to the flash memory.
 // warning The flash memory area receiving the data must be erased.
 // warning The @p buffer must be at least @p size bytes long.
 // param address First address in the flash memory where to copy the data to.
 // param buffer Buffer containing the data to copy.
 // param size Size of the data to be copied in bytes.
 // return EE_FLASH_RETURN_SUCCESS         No error.
 // return EE_FLASH_RETURN_NO_PERMISSION   Access denied.
  int EE_flashWrite( flashaddr_t address, const char* buffer, size_t size);


#ifdef __cplusplus
}
#endif

#endif // __FLASH_H__

