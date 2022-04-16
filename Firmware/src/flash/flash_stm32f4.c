
#include <string.h>

#include "flash_stm32.h"



#if defined(STM32F4XX)


//#define FLASH_MAX_NSECTOR	8	// STM32F401xE
#define FLASH_MAX_NSECTOR	12	// STM32F407x

int EE_flashWaitWhileBusy()
{
	while ( FLASH->SR & FLASH_SR_BSY )
	{
		__NOP();
	}

//	if ( FLASH->SR & (FLASH_SR_PGSERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR | FLASH_SR_OPERR ) )
	if ( FLASH->SR & (FLASH_SR_PGSERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR  ) )
	{
		return EE_FLASH_RETURN_WAIT_ERROR;
	}

	return EE_FLASH_RETURN_SUCCESS;
}


size_t EE_flashSectorSize(flashsector_t sector)
{
    if ( /*sector >= 0 &&*/ sector <= 3)
        return 16 * 1024;
    else if (sector == 4)
        return 64 * 1024;
    else if (sector >= 5 && sector < FLASH_MAX_NSECTOR )
        return 128 * 1024;
    return 0;
}

flashaddr_t EE_flashSectorBegin(flashsector_t sector)
{
    flashaddr_t address = FLASH_BASE;
    while (sector > 0)
    {
        --sector;
        address += EE_flashSectorSize(sector);
    }
    return address;
}

flashaddr_t EE_flashSectorEnd(flashsector_t sector)
{
    return EE_flashSectorBegin(sector + 1);
}

flashsector_t EE_flashSectorAt(flashaddr_t address)
{
    flashsector_t sector = 0;
    while (address >= EE_flashSectorEnd(sector))
        ++sector;
    return sector;
}


// Unlock the flash memory for write access.
// CH_SUCCESS  Unlock was successful.
// CH_FAILED    Unlock failed.
static bool EE_flashUnlock(void)
{
    /* Check if unlock is really needed */
    if (!(FLASH->CR & FLASH_CR_LOCK))
        return CH_SUCCESS;

    /* Write magic unlock sequence */
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;

    /* Check if unlock was successful */
    if (FLASH->CR & FLASH_CR_LOCK)
        return CH_FAILED;
    return CH_SUCCESS;
}

// Lock the flash memory for write access.
#define EE_flashLock() { FLASH->CR |= FLASH_CR_LOCK; }

int EE_flashSectorErase(flashsector_t sector)
{
	if ( sector >= FLASH_MAX_NSECTOR )
		return EE_FLASH_RETURN_BAD_NSECTOR;

    // Unlock flash for write access
    if( EE_flashUnlock() == CH_FAILED)
        return EE_FLASH_RETURN_NO_PERMISSION;

    // Wait for any busy flags.
    EE_flashWaitWhileBusy();

    // Setup parallelism before any program/erase
    FLASH->CR &= ~FLASH_CR_PSIZE_MASK;
    FLASH->CR |= FLASH_CR_PSIZE_VALUE;

    // Start deletion of sector.
    // SNB(3:1) is defined as:
    // 0000 sector 0
    // 0001 sector 1
    // ...
    // 0111 sector 7 - MAX_NSECTOR for e.g. stm32f401xE
    // ...
    // 1011 sector 11 - MAX_NSECTOR for  e.g. stm32f405/f407
    // others not allowed
    FLASH->CR &= ~(FLASH_CR_SNB_0 | FLASH_CR_SNB_1 | FLASH_CR_SNB_2 | FLASH_CR_SNB_3);
    if (sector & 0x1) FLASH->CR |= FLASH_CR_SNB_0;
    if (sector & 0x2) FLASH->CR |= FLASH_CR_SNB_1;
    if (sector & 0x4) FLASH->CR |= FLASH_CR_SNB_2;
    if (sector & 0x8) FLASH->CR |= FLASH_CR_SNB_3;
    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR |= FLASH_CR_STRT;

    // Wait until it's finished.
    EE_flashWaitWhileBusy();

    // Sector erase flag does not clear automatically.
    FLASH->CR &= ~FLASH_CR_SER;

    // Lock flash again
    EE_flashLock();

    // Check deleted sector for errors
    if ( EE_flashIsErased( EE_flashSectorBegin(sector), EE_flashSectorSize(sector)) == FALSE )
        return EE_FLASH_RETURN_BAD_FLASH;  // Sector is not empty despite the erase cycle!

    // Successfully deleted sector
    return EE_FLASH_RETURN_SUCCESS;
}

int EE_flashErase(flashaddr_t address, size_t size)
{
    while (size > 0)
    {
        flashsector_t sector = EE_flashSectorAt(address);
        int err = EE_flashSectorErase(sector);
        if (err != EE_FLASH_RETURN_SUCCESS)
            return err;
        address = EE_flashSectorEnd(sector);
        size_t sector_size = EE_flashSectorSize(sector);
        if (sector_size >= size)
            break;
        else
            size -= sector_size;
    }

    return EE_FLASH_RETURN_SUCCESS;
}

bool EE_flashIsErased(flashaddr_t address, size_t size)
{
    // Check for default set bits in the flash memory
    // For efficiency, compare flashdata_t values as much as possible,
    // then, fallback to byte per byte comparison.
    while (size >= sizeof(flashdata_t))
    {
        if (*(volatile flashdata_t*)address != (flashdata_t)(-1)) // flashdata_t being unsigned, -1 is 0xFF..FF
            return FALSE;
        address += sizeof(flashdata_t);
        size -= sizeof(flashdata_t);
    }
    while (size > 0)
    {
        if (*(char*)address != 0xff)
            return FALSE;
        ++address;
        --size;
    }

    return TRUE;
}

bool EE_flashCompare(flashaddr_t address, const char* buffer, size_t size)
{
    // For efficiency, compare flashdata_t values as much as possible,
    // then, fallback to byte per byte comparison.
    while (size >= sizeof(flashdata_t))
    {
        if (*(volatile flashdata_t*)address != *(flashdata_t*)buffer)
            return FALSE;
        address += sizeof(flashdata_t);
        buffer += sizeof(flashdata_t);
        size -= sizeof(flashdata_t);
    }
    while (size > 0)
    {
        if (*(volatile char*)address != *buffer)
            return FALSE;
        ++address;
        ++buffer;
        --size;
    }

    return TRUE;
}

int EE_flashRead(flashaddr_t address, char* buffer, size_t size)
{
    memcpy(buffer, (char*)address, size);
    return EE_FLASH_RETURN_SUCCESS;
}

static void EE_flashWriteData(flashaddr_t address, const flashdata_t data)
{
    // Enter flash programming mode
    FLASH->CR |= FLASH_CR_PG;

    // Write the data
    *(flashdata_t*)address = data;

    // Wait for completion
    EE_flashWaitWhileBusy();

    // Exit flash programming mode
    FLASH->CR &= ~FLASH_CR_PG;
}

int EE_flashWrite(flashaddr_t address, const char* buffer, size_t size)
{
    // Unlock flash for write access
    if ( EE_flashUnlock() == CH_FAILED )
        return EE_FLASH_RETURN_NO_PERMISSION;

    // Wait for any busy flags
    EE_flashWaitWhileBusy();

    // Setup parallelism before any program/erase
    FLASH->CR &= ~FLASH_CR_PSIZE_MASK;
    FLASH->CR |= FLASH_CR_PSIZE_VALUE;

    // Check if the flash address is correctly aligned
    size_t alignOffset = address % sizeof(flashdata_t);
    if (alignOffset != 0)
    {
        // Not aligned, thus we have to read the data in flash already present
        // and update them with buffer's data

        // Align the flash address correctly
        flashaddr_t alignedFlashAddress = address - alignOffset;

        // Read already present data
        flashdata_t tmp = *(volatile flashdata_t*)alignedFlashAddress;

        // Compute how much bytes one must update in the data read
        size_t chunkSize = sizeof(flashdata_t) - alignOffset;
        if (chunkSize > size)
            chunkSize = size; // this happens when both address and address + size are not aligned

        // Update the read data with buffer's data
        memcpy((char*)&tmp + alignOffset, buffer, chunkSize);

        // Write the new data in flash
        EE_flashWriteData(alignedFlashAddress, tmp);

        // Advance
        address += chunkSize;
        buffer += chunkSize;
        size -= chunkSize;
    }

    // Now, address is correctly aligned. One can copy data directly from
    // buffer's data to flash memory until the size of the data remaining to be
    // copied requires special treatment.
    while (size >= sizeof(flashdata_t))
    {
        EE_flashWriteData(address, *(const flashdata_t*)buffer);
        address += sizeof(flashdata_t);
        buffer += sizeof(flashdata_t);
        size -= sizeof(flashdata_t);
    }

    // Now, address is correctly aligned, but the remaining data are to
    // small to fill a entier flashdata_t. Thus, one must read data already
    // in flash and update them with buffer's data before writing an entire
    // flashdata_t to flash memory.
    if (size > 0)
    {
        flashdata_t tmp = *(volatile flashdata_t*)address;
        memcpy(&tmp, buffer, size);
        EE_flashWriteData(address, tmp);
    }

    // Lock flash again
    EE_flashLock();

    return EE_FLASH_RETURN_SUCCESS;
}

#endif

