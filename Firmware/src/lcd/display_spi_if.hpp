/*
 * display_spi_if.hpp
 *
 *  Created on: 18 Mar 2021
 *      Author: jmajew
 */

#ifndef __LCD_DISPLAY_SPI_IF_HPP__
#define __LCD_DISPLAY_SPI_IF_HPP__


#include "system/sysdecl.hpp"
#include "ssd1306_conf.h"

#ifdef LCD_IF_SPI

////////////////////////////////////////////////////////////////////////////////////////////////////
// class DispSpiInterface
// 4-pin connection: SCK, MOSI, MISO, CS, D/C
////////////////////////////////////////////////////////////////////////////////////////////////////

#define SWAP16(x)	(__builtin_bswap16(x))


#define BUFFER_SIZE		1024

class DispSpiInterface //: public DispInterface
{
public:
	DispSpiInterface( SPIDriver* pdrv) : mpDrv(pdrv)	{}

	void	Start();
	void	Reset();

	void	AquireBus()		{ spiAcquireBus( mpDrv); spiSelect(mpDrv); }
	void	ReleaseBus()	{ spiReleaseBus( mpDrv); spiUnselect(mpDrv); }

	msg_t	WriteCommand( uint8_t byte);	// polled spiSend
	msg_t	WriteData_fast( uint8_t* buffer, size_t buff_size); // polled spiSend

	msg_t	WriteData( uint8_t* buffer, size_t buff_size);
	msg_t	WriteDataBurst( uint16_t color, size_t data_size);
	msg_t	ReadData( uint8_t* buf, size_t pix_count);

	uint8_t*			pBuffer()		{ return mTxBuffer; }
	constexpr uint16_t	bufferSize()	{ return BUFFER_SIZE; }

private:
	SPIDriver*	mpDrv;
	uint8_t		mTxBuffer[BUFFER_SIZE];
};

#endif // LCD_IF_SPI

#endif // __LCD_DISPLAY_SPI_IF_HPP__
