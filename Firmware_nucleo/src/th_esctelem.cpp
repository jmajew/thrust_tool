/*
 * th_serial.cpp
 *
 *  Created on: 20 Feb 2021
 *      Author: jmajew
 */

#include "th_esctelem.hpp"
#include "sbuf.hpp"
#include "master.hpp"


#define ESC_SERIAL_BYTE_COUNT		10

static const SerialConfig sdcfg =
{
  115200,
  0,
  USART_CR2_STOP1_BITS,
  0
};


uint8_t buffer[16];

uint8_t update_crc8( uint8_t crc, uint8_t crc_seed )
{
	uint8_t crc_u, i;
	crc_u = crc;
	crc_u ^= crc_seed;
	for ( i = 0; i < 8; i++ )
		crc_u = (crc_u & 0x80) ? 0x7 ^ (crc_u << 1) : (crc_u << 1);
	return (crc_u);
}

uint8_t get_crc8( uint8_t *Buf, uint8_t BufLen )
{
	uint8_t crc = 0, i;
	for ( i = 0; i < BufLen; i++ )
		crc = update_crc8(Buf[i], crc);
	return (crc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// class EscTelemThread
////////////////////////////////////////////////////////////////////////////////////////////////////

bool EscTelemThread::processBuffer()
{

	if (mpData)
	{
		EscTelem_Data* pdat = &mpData->grpEscTelem;
		SBuf sbin( buffer, ESC_SERIAL_BYTE_COUNT);

		uint8_t hi, low;
		pdat->mux.lock();
		pdat->temp = sbin.ReadU8();

		hi = sbin.ReadU8();
		low = sbin.ReadU8();
		pdat->volt = ((uint16_t)hi << 8) | low;

		hi = sbin.ReadU8();
		low = sbin.ReadU8();
		pdat->curr = ((uint16_t)hi << 8) | low;

		hi = sbin.ReadU8();
		low = sbin.ReadU8();
		hi = sbin.ReadU8();
		low = sbin.ReadU8();
		pdat->rpm = ((uint16_t)hi << 8) | low;

		pdat->mux.unlock();

		low = sbin.ReadU8(); // message crc
		uint8_t crc = get_crc8( buffer, ESC_SERIAL_BYTE_COUNT-1);

//		dbg_printf( "-- crc = %d %d \r\n", low, crc );

		if ( low == crc ) // ok
		{
			dbg_printf( "EscTelemThread ok! - %d \r\n", pdat->rpm );

			return true;
		}
		else
		{
			dbg_puts("EscTelemThread - crc ERROR !!!\r\n");
		}
	}

	return false;
}


void EscTelemThread::main()
{
	chRegSetThreadName( "EscTelemThread");

	dbg_puts("EscTelemThread - main\r\n");

	ASSERT( mpSD != nullptr);

	sdStart( &SD_ESC_TELEM, &sdcfg);

//	    sdStart( &RX_SD, &rxSerialCfg);

//	bool bGo = true;


	event_listener_t esc_el;

//		chEvtRegister( chnGetEventSource( &MSP_SERIAL), &msp_el, 1);

	chEvtRegisterMask( chnGetEventSource( mpSD ), &esc_el, EVENT_MASK(1) );

	bool bRestart = true;
	int count = 0;

	while ( ! shouldTerminate() )
	{
		eventmask_t mask = waitOneEventTimeout( EVENT_MASK(1), TIME_MS2I(100) );

		if ( mask & EVENT_MASK(1) )
		{
			eventflags_t flags_serial = chEvtGetAndClearFlags( &esc_el);
			dbg_puts("EscTelem :: serial event captured\r\n");


			if (flags_serial & CHN_INPUT_AVAILABLE)
			{
				msg_t charbuf;
				while ( (charbuf = chnGetTimeout( mpSD, TIME_MS2I(10) )) != Q_TIMEOUT )
				{
					dbg_printf(" -- rpm-serial received: \"%d\"\r\n", (int)charbuf);

					if ( bRestart)
					{
						bRestart = false;
						count = 0;
					}

					buffer[count++] = (uint8_t)charbuf;

					if (count == ESC_SERIAL_BYTE_COUNT)
					{
//						dbg_printf("buf: %d %d %d %d %d %d %d %d %d\r\n",
//								buffer[0], buffer[1], buffer[2], buffer[3], buffer[4],
//								buffer[5], buffer[6], buffer[7], buffer[8], buffer[9] );
						bRestart = true;
						processBuffer();	// TODO :: make sure reading is in sync with data
					}
					else if (count > ESC_SERIAL_BYTE_COUNT)
					{
						dbg_puts( "error :: count > ESC_SERIAL_BYTE_COUNT");
						// bRestart should be already true
						// TODO :: fix eventual overflow of buffer
					}

				}

				if (charbuf == Q_TIMEOUT)
				{
					bRestart = true;
				}

			}

			if (flags_serial & (SD_PARITY_ERROR | SD_FRAMING_ERROR | SD_OVERRUN_ERROR |
					SD_NOISE_ERROR | SD_BREAK_DETECTED | SD_QUEUE_FULL_ERROR ))
			{
				// Some receive error happened.
				dbg_printf( "!!! Rpm-serial error: %d \r\n", flags_serial );
			}

//			if (flags_serial & (SD_QUEUE_FULL_ERROR) )
//			{
//				chSysLock();
//				iqResetI( &SD_ESC_TELEM.iqueue);
//				chSysUnlock();
//			}
		}

//	    	sleep( TIME_MS2I( 100));  // HACK ::
	}

	chEvtUnregister( chnGetEventSource( mpSD ), &esc_el );
	sdStop( &SD_ESC_TELEM);

	dbg_puts("EscTelemThread - STOPPED\r\n");

	// invalidate data
	mpData->grpEscTelem.Reset();

	exit( MSG_OK );
}


////////////////////////////////////////////////////////////////////////////////////////////////////




