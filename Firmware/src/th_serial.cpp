/*
 * th_serial.cpp
 *
 *  Created on: 20 Feb 2021
 *      Author: jmajew
 */

#include "th_serial.hpp"
#include "cli.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class THSerialThread
////////////////////////////////////////////////////////////////////////////////////////////////////

ThspMessage	command;
ThspMessage	reply;

void SerialThread::readMessage()
{
	msg_t charbuf;
	while ( (charbuf = chnGetTimeout( mpPort->pSD(), TIME_MS2I(10) )) != Q_TIMEOUT )
	{
		dbg_printf(" -- received: \"%d\"\r\n", (int)charbuf);

		const bool bmsp = mpPort->ProcessReceivedData( &command, charbuf);

		if ( !bmsp )
		{
			// non  thsp data
			dbg_puts( " -- non-thsp data\r\n");
		}

		if ( mpPort->GetState() == THSP_COMMAND_RECEIVED)
		{
			dbg_puts( " -- received message\r\n");
			break;
		}
	}
}

void SerialThread::execMessage()
{
	if ( command.packetType == THSP_PACKET_COMMAND)
	{
		ETHSerialResult status = mpPort->ProcessReceivedCommand( &command, &reply);
		(void)status;

	}
	else if ( command.packetType == THSP_PACKET_REPLY)
	{
		dbg_puts("ERROR - execMessage :: packetType should be command\r\n");
	}
}

void SerialThread::sendMessage()
{
	if ( mpPort->PackPackage( &reply) )
	{
		// Send packet
//			msg_t n = chnWriteTimeout( mpPort->pSD(), reply.sbuf.GetPBeg(), reply.sbuf.Length(), TIME_MS2I(1));
		msg_t n = chnWriteTimeout( mpPort->pSD(), mpPort->rSBuf().GetPBeg(), mpPort->rSBuf().Length(), TIME_MS2I(3));

		dbg_puts( " -- reply sent\r\n" );

		if ( static_cast<size_t>( n) != mpPort->rSBuf().Length() )
		{
			// failed to transfer
			dbg_puts( " -- failed to transfer\r\n" );
		}
	}
	else
	{
		dbg_puts( " -- failed to pack reply msg\r\n");
	}
}

void SerialThread::main()
{
	chRegSetThreadName( "THSerial");

	dbg_puts("THSerialThread - main\r\n");

	ASSERT( mpPort != nullptr);

//	    sdStart( &RX_SD, &rxSerialCfg);


	event_listener_t msp_el;
//	event_listener_t data_rdy_el;
	chibi::EventListener datardy_el;
	chibi::EventListener shell_el;

//		chEvtRegister( chnGetEventSource( &MSP_SERIAL), &msp_el, 1);
	chEvtRegisterMask( chnGetEventSource( mpPort->pSD() ), &msp_el, EVENT_MASK(1) );
	evtDataRdy.registerMask( &datardy_el, EVENT_MASK(2));
	evtShell.registerMask( &shell_el, EVENT_MASK(3));

	while ( ! shouldTerminate() )
	{
		eventflags_t flags_serial = 0;	//uint32_t
		eventflags_t flags_measure = 0;	//uint32_t
		eventflags_t flags_shell = 0;	//uint32_t
		(void)flags_shell;

		eventmask_t mask = waitOneEventTimeout( EVENT_MASK(1)|EVENT_MASK(2)|EVENT_MASK(3), TIME_MS2I(100));
//		eventmask_t mask = chEvtWaitOneTimeout( EVENT_MASK(1) | EVENT_MASK(2), TIME_MS2I(100));

		if ( mask & EVENT_MASK(1) )
		{
			flags_serial = chEvtGetAndClearFlags( &msp_el);
			dbg_puts("serial event captured\r\n");
		}

		if ( mask & EVENT_MASK(2) )
		{
			palToggleLine( LINE_DEBUG_1 );
			flags_measure = datardy_el.getAndClearFlags();
//			dbg_puts("data_rdy signal captured\r\n");
			//continue;
		}

		if ( mask & EVENT_MASK(3) )
		{
			// starts shell and waits until it is stopped
			flags_shell = shell_el.getAndClearFlags();
			shellStart( (BaseSequentialStream*)mpPort->pSD() );
		}

		if (flags_serial & CHN_INPUT_AVAILABLE)
		{
			if ( mpPort->GetState() == THSP_IDLE)
			{
				//dbg_puts( "CHN_INPUT_AVAILABLE\r\n");
				readMessage();
			}

			//chibi::BaseThread::sleep( TIME_MS2I( 10));
			if ( mpPort->GetState() == THSP_COMMAND_RECEIVED)
			{
				execMessage();
				sendMessage();
				mpPort->SetState( THSP_IDLE);

			    switch ( command.command )
				{
				case THSP_DATASTREAM_START:
					dbg_puts( "-- Data_stream started");
					this->SetMode( ESerialMode::Data_stream );
					break;

				case THSP_DATASTREAM_STOP:
					dbg_puts( "-- Data_stream stopped");
					this->SetMode( ESerialMode::Command );
					break;

				};

//	            	mpPort->SetState( THSP_REPLY_RDY);
			}
			else
			{
				// reset port
				mpPort->SetState(THSP_IDLE); // TODO :: check this
				dbg_puts("-- reseting thsp_port !!!");
			}
		}

		if ( true && (flags_measure & SIG_DATARDY)  && mMode == ESerialMode::Data_stream )
		{
			palToggleLine( LINE_DEBUG_1 );	// DEBUG_line
			dbg_puts("signal data_rdy captured\r\n");

			command.packetType = THSP_PACKET_COMMAND;
			command.command = THSP_SEND_DATA;
			command.dataSize = 0;

			execMessage();
			sendMessage();
			mpPort->SetState( THSP_IDLE);
			palToggleLine( LINE_DEBUG_1 );	// DEBUG_line
		}

		//			if (flags & CHN_OUTPUT_EMPTY)
		//			{
		//				// Data sent, you may transmit from here.
		//				if ( mpPort->GetState() == THSP_REPLY_RDY)
		//				{
		//					sendMessage();
		//					mpPort->SetState( THSP_IDLE);
		//				}
		//	//			chprintf( (BaseSequentialStream*)DEBUG_CHP, "Can transmit\r\n");
		//			}

		//SD_QUEUE_FULL_ERROR 1024
		if (flags_serial & (SD_PARITY_ERROR | SD_FRAMING_ERROR | SD_OVERRUN_ERROR |
				SD_NOISE_ERROR | SD_BREAK_DETECTED | SD_QUEUE_FULL_ERROR ))
		{
			// Some receive error happened.
			dbg_printf( "!!! Thsp-serial error: %d \r\n", flags_serial );
		}

//	    	sleep( TIME_MS2I( 100));  // HACK ::
	}

	exit( MSG_OK );
}


////////////////////////////////////////////////////////////////////////////////////////////////////




