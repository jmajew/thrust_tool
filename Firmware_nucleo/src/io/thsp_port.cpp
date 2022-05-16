/*
 * thsp_port.cpp
 *
 *  Created on: 10.03.2019
 *      Author: jmajew
 */

#include "io/thsp_port.hpp"
#include "io/thsp_processor.hpp"


ETHSerialResult ThspPort::ProcessReceivedCommand( ThspMessage *command, ThspMessage *reply )
{
	(void)command;
	(void)reply;

	SBuf replyBuf( reply->data);

	SBuf commandBuf( command->data);
	commandBuf.SetPEnd( command->data + command->dataSize );

//	reply->sbuf.SetPBeg( reply->buffer);
//    reply->sbuf.SetPEnd( reply->buffer + THSP_MSG_DATA_OFFSET);
    reply->command = command->command;
    reply->packetType = THSP_PACKET_REPLY;

    if ( !mpProc)
    {
    	return THSP_RESULT_CMD_UNKNOWN;
    }

    ETHSerialResult status = mpProc->ProcessCommand( command->command, commandBuf, replyBuf);
    reply->dataSize = replyBuf.Length();

    if ( status == THSP_RESULT_CMD_UNKNOWN )
    {
    	// try another cmd processor
    }

    return status;

//    if ( status < THSP_RESULT_OK)
//    {
//    	// message not decoded properly
//    	return false;
//    }
//
//    return true;
}


