/*
 * thsp_handler.hpp
 *
 *  Created on: 18.03.2019
 *      Author: jmajew
 */

#ifndef __IO_THSP_HANDLER_HPP__
#define __IO_THSP_HANDLER_HPP__

#ifdef WIN32
	#include "sysdecl.h"
	#include "thsp_packet.hpp"
#else
	#include "system/sysdecl.hpp"
	#include "util/util.hpp"
	#include "io/thsp_packet.hpp"
#endif 

enum ETHSerialVersion
{
    THSP_V1          = 0,
    THSP_VERSION_COUNT
};

enum ETHSerialState
{
    THSP_IDLE,
    THSP_HEADER_START,
    THSP_HEADER_M,
    THSP_HEADER_X,

    THSP_HEADER_V1,
    THSP_PAYLOAD_V1,
    THSP_CHECKSUM_V1,

    THSP_COMMAND_RECEIVED,
	THSP_REPLY_RDY
};


class ThspProcessor;

struct ThspBuffer
{
	uint8_t checksum1;
	SBuf	sbuf;
    uint8_t data[THSP_MAX_MESSAGE_SIZE];
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThspHandler
////////////////////////////////////////////////////////////////////////////////////////////////////
class ThspHandler
{
public:
	ThspHandler() : mState(THSP_IDLE)						{}

//	void				SetProcessor( ThspProcessor* ptr)	{ mpProc = ptr; }

	ETHSerialState		GetState() const					{ return mState; }
	void				SetState( ETHSerialState st)		{ mState = st; }
	void				Reset()								{ mState = THSP_IDLE; }

	ETHSerialVersion	GetVer() const						{ return mVer; }
	void				SetVer( ETHSerialVersion ver)		{ mVer = ver; }

	SBuf&				rSBuf()		{ return mBuffer.sbuf; }

	bool	ProcessReceivedData( ThspMessage* pack, uint8_t c);
	bool	PackPackage( const ThspMessage* msg);

//	bool	ProcessReceivedCommand( ThspPacket *command, ThspPacket *reply );
//	bool	PackReply( ThspPacket* reply);

protected:
//	ThspProcessor*	mpProc;
	ETHSerialVersion	mVer;

	ETHSerialState 		mState;
    uint16_t 			mOffset;
    ThspBuffer			mBuffer;

//    uint8_t 			mReceiveBuffer[THSP_MAX_MESSAGE_SIZE];
//    uint8_t 			mSendBuffer[THSP_MAX_MESSAGE_SIZE];


	//mspPendingSystemRequest_e pendingRequest;
};


#endif // __IO_THSP_HANDLER_HPP__
