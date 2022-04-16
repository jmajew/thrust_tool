/*
 * io.cpp
 *
 *  Created on: Mar 11, 2019
 *      Author: jmajew
 */

#include "io/io.hpp"
#include "system/setup.h"
#include "io/usbcfg.h"


//static SerialConfig sdacfg =
//{
//    115200,                                 /* 115200 baud rate */
//    USART_CR1_9BIT_WORD | USART_CR1_PARITY_SET | USART_CR1_EVEN_PARITY,
//    USART_CR2_STOP1_BITS | USART_CR2_LINEN,
//    0
//};

static const SerialConfig sdacfg =
{
	250000,
//	115200,
	0,
	USART_CR2_STOP1_BITS,
	0
};

static const SerialConfig sdbcfg =
{
  115200,
  0,
  USART_CR2_STOP1_BITS,
  0
};

void	IoMaster::Start()
{
	// Initializes a serial-over-USB CDC driver.
	sduObjectInit( &SDU1);
	sduStart( &SDU1, &serusbcfg);

	// Activates the USB driver and then the USB bus pull-up on D+.
	// Note, a delay is inserted in order to not have to disconnect the cable
	// after a reset.
	usbDisconnectBus( serusbcfg.usbp);

	//chThdSleepMilliseconds(1500);
	chibi::BaseThread::sleep( TIME_MS2I( 1500) );

	usbStart( serusbcfg.usbp, &usbcfg);
	usbConnectBus( serusbcfg.usbp);


	sdStart( &SDA, &sdacfg);

//	sdStart( &SDB, &sdbcfg);
	
//	mpIOOutput 		= (BaseAsynchronousChannel*)&SD_OUTPUT;
//	mpIOSerialComm	= (BaseAsynchronousChannel*)&SD_SERIAL;
}


//IoMaster ioMaster( (BaseAsynchronousChannel*)&SD_OUTPUT, (BaseAsynchronousChannel*)&SD_SERIAL );

//cio::costream			ccout( &ioMaster.rOutput() );


