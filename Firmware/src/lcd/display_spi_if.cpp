/*
 * display_spi_if.cpp
 *
 *  Created on: 18 Mar 2021
 *      Author: jmajew
 */

#include "system/setup.h"
#include "lcd/display_spi_if.hpp"
#include "lcd/ssd1306_commands.h"


#ifdef LCD_IF_SPI

// fPCLL1 = 42MHz
//	BR[2:0]: Baud rate control
//	000: fPCLK/2
//	001: fPCLK/4
//	010: fPCLK/8
//	011: fPCLK/16
//	100: fPCLK/32
//	101: fPCLK/64
//	110: fPCLK/128
//	111: fPCLK/256

//static bool bSpiWait = false;
//static void spicb(SPIDriver *spip)
//{
//	(void)spip;
//	chSysLockFromISR();
////	spiUnselectI(spip);
//	bSpiWait = false;
//	chSysUnlockFromISR();
//}

// SPI_CR1_DFF = 0 -> 8 bit, 1 -> 16 bit
static const SPIConfig spicfg =
{
	FALSE,
	NULL,				// spicb,
	LINE_LCD_SPI_CS,	// PAL_LINE(GPIOA,4),		// LINE_LCD_SPI_CS,
	SPI_CR1_BR_1,
	//SPI_CR1_BR_0,		//	SPI_CR1_BR_1 | SPI_CR1_BR_2,
	0					// SPI_CR2_SSOE	//0
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class DispSpiInterface
////////////////////////////////////////////////////////////////////////////////////////////////////



void	DispSpiInterface::Start()
{
//	mpDrv = &SPID3;

	palSetLine( LINE_LCD_RESET);

	spiStart( mpDrv, &spicfg);
}

void	DispSpiInterface::Reset()
{
	palClearLine( LINE_LCD_RESET);
	chibi::BaseThread::sleep( TIME_MS2I(5));
	palSetLine( LINE_LCD_RESET);
}

//msg_t	DispSpiInterface::WriteCommand(uint8_t byte)
//{
////	spiAcquireBus( mpDrv);
//	palClearLine( LINE_LCD_SPI_DC);
////	spiSend( mpDrv, 1, &byte);
//	spiStartSend( mpDrv, 1, &byte);		// HACK ::
////	spiReleaseBus( mpDrv);
//
//	return MSG_OK;
//}
//
//msg_t	DispSpiInterface::WriteData( uint8_t* buffer, size_t buff_size)
//{
////	spiAcquireBus( mpDrv);
//	palSetLine( LINE_LCD_SPI_DC);
//	spiSend( mpDrv, buff_size, buffer);
////	spiReleaseBus( mpDrv);
//
//	return MSG_OK;
//}

msg_t	DispSpiInterface::WriteCommand( uint8_t byte)
{
#ifdef LCD_CS_BY_SPIIF
	AquireBus();	// needed for SSD1306
#endif

	palClearLine( LINE_LCD_SPI_DC);

	uint16_t frame = byte;
	spiPolledExchange( mpDrv, frame);

#ifdef LCD_CS_BY_SPIIF
	ReleaseBus();
#endif

	return MSG_OK;

//	bSpiWait = true;
//	spiStartSend( mpDrv, 1, &byte);
//
//	// polled wait for transmission end
//	while ( bSpiWait)
//		asm("NOP");
}

msg_t	DispSpiInterface::WriteData( uint8_t* buffer, size_t buff_size)	// safe
{
#ifdef LCD_CS_BY_SPIIF
	AquireBus();	// needed for SSD1306
#endif

	palSetLine( LINE_LCD_SPI_DC);
	spiSend( mpDrv, buff_size, buffer);

#ifdef LCD_CS_BY_SPIIF
	ReleaseBus();
#endif

	return MSG_OK;
}

msg_t	DispSpiInterface::WriteData_fast( uint8_t* buffer, size_t buff_size)
{
#ifdef LCD_CS_BY_SPIIF
	AquireBus();	// needed for SSD1306
#endif
	palSetLine( LINE_LCD_SPI_DC);

	for (size_t i=0; i<buff_size; ++i)
	{
		uint16_t frame = *(buffer++);
		spiPolledExchange( mpDrv, frame);
	}

#ifdef LCD_CS_BY_SPIIF
	ReleaseBus();
#endif

	return MSG_OK;

	//	bSpiWait = true;
//	spiStartSend( mpDrv, buff_size, buffer);
//
//	// polled wait for transmission end
//	while ( bSpiWait)
//		asm("NOP");
}

msg_t	DispSpiInterface::WriteDataBurst( uint16_t color, size_t data_size)
{
	int32_t	byte_count = data_size * 2;

	// fill buffer
	size_t fill_count = byte_count < BUFFER_SIZE ? byte_count : BUFFER_SIZE;
	std::fill( (uint16_t*)mTxBuffer, (uint16_t*)(mTxBuffer+fill_count), SWAP16( color) );

	palSetLine( LINE_LCD_SPI_DC);

	do
	{
		if ( byte_count <= BUFFER_SIZE )
		{
			if ( byte_count < 100 )
				WriteData_fast( mTxBuffer, byte_count);
			else
				WriteData( mTxBuffer, byte_count);

			break;
		}
		else
		{
			WriteData( mTxBuffer, BUFFER_SIZE);
			byte_count -= BUFFER_SIZE;
		}
	}
	while( true);

	return MSG_OK;
}

#ifndef ILI9341_COLOR
#define ILI9341_COLOR(r, g, b) \
			((((uint16_t)b) >> 3) | \
            ((((uint16_t)g) << 3) & 0x07E0) | \
            ((((uint16_t)r) << 8) & 0xf800))
#endif

msg_t	DispSpiInterface::ReadData( uint8_t* buf, size_t pix_count)
{
	static const uint8_t dummy = 0xff;

	uint8_t col[3];

	palSetLine( LINE_LCD_SPI_DC);

	spiSend( mpDrv, 1, &dummy);
//	spiReceive( mpDrv, 1, &red);

	for (uint32_t i = 0; i < pix_count; ++i)
	{
		spiReceive( mpDrv, 3, col);

		uint16_t color = ILI9341_COLOR(col[0], col[1], col[2]);
		*(uint16_t*)(buf+2*i) = SWAP16( color);
	}

	return MSG_OK;
}

#endif // LCD_IF_SPI


