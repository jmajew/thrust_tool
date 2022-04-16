/*
 * display_if.cpp
 *
 *  Created on: Mar 12, 2021
 *      Author: jmajew
 */
 
 
#include "system/setup.h"
#include "lcd/display_i2c_if.hpp"
#include "lcd/ssd1306_commands.h"


#ifdef LCD_IF_I2C


static const I2CConfig i2cfg =
{
    OPMODE_I2C,
    400000,		// 400000
//    STD_DUTY_CYCLE,
   FAST_DUTY_CYCLE_2,
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class DispI2cInterface
////////////////////////////////////////////////////////////////////////////////////////////////////

void	DispI2cInterface::Start()
{
	i2cStart( mpDrv, &i2cfg);
}


msg_t	DispI2cInterface::WriteCommand(uint8_t byte)
{
	mTxBuffer[0] = 0x00;	// d/c bit 0 -> command
	mTxBuffer[1] = byte;

	i2cAcquireBus( mpDrv);
	msg_t msg = i2cMasterTransmitTimeout( mpDrv, SSD1306_I2C_ADDR, mTxBuffer, 2, NULL, 0, 100);
	i2cReleaseBus( mpDrv);

	return msg;
}

msg_t	DispI2cInterface::WriteData( uint8_t* buffer, size_t buff_size)
{
	mTxBuffer[0] = 0x40;	// d/c bit 1 -> data
	memcpy( mTxBuffer+1, buffer, buff_size );

	i2cAcquireBus( mpDrv);
	msg_t msg = i2cMasterTransmitTimeout( mpDrv, SSD1306_I2C_ADDR, mTxBuffer, buff_size+1, NULL, 0, 100);
	i2cReleaseBus( mpDrv);

	return msg;
}


#endif // LCD_IF_I2C


