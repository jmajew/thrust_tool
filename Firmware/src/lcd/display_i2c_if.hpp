/*
 * display_if.hpp
 *
 *  Created on: Mar 12, 2021
 *      Author: jmajew
 */
 
 #ifndef __LCD_DISPLAY_I2C_IF_HPP__
#define __LCD_DISPLAY_I2C_IF_HPP__

#include "system/sysdecl.hpp"
#include "ssd1306_conf.h"

#ifdef LCD_IF_I2C

////////////////////////////////////////////////////////////////////////////////////////////////////
// class DispI2cInterface
// 2-pin connection: SCL, SDA
////////////////////////////////////////////////////////////////////////////////////////////////////
class DispI2cInterface //: public DispInterface
{
public:
	DispI2cInterface( I2CDriver* pdrv) : mpDrv(pdrv)
	{
	}

	void	Start();
//	void	Reset()	{};
	void	AquireBus()		{ i2cAcquireBus( mpDrv); }
	void	ReleaseBus()	{ i2cReleaseBus( mpDrv); }

	msg_t	WriteCommand( uint8_t byte);
	msg_t	WriteData( uint8_t* buffer, size_t buff_size);

private:
	I2CDriver*	mpDrv;
	uint8_t		mTxBuffer[SSD1306_BUFFER_SIZE+1];
};

#endif // LCD_IF_I2C

#endif // __LCD_DISPLAY_I2C_IF_HPP__

