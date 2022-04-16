/*
 * thsp_packet.hpp
 *
 *  Created on: 20 Feb 2021
 *      Author: jmajew
 */

#ifndef __IO_THSP_PACKET_HPP__
#define __IO_THSP_PACKET_HPP__

#ifdef WIN32
	#include "sysdecl.h"
	
	#include "thsp_protocol.h"
	#include "sbuf.hpp"
#else
	#include "system/sysdecl.hpp"
	#include "util/util.hpp"
	
	#include "io/thsp_protocol.h"
	#include "util/sbuf.hpp"
#endif 

//#define THSP_MAX_DATA_SIZE	64

#define THSP_PORT_INBUF_SIZE		192
#define THSP_PORT_OUTBUF_SIZE		256

#define THSP_MAX_MESSAGE_SIZE		256
#define THSP_MSG_DATA_OFFSET		5





enum EPacketType
{
	THSP_PACKET_UNKNOWN,
    THSP_PACKET_COMMAND,
    THSP_PACKET_REPLY,
    THSP_PACKET_ERROR
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThspMessage
////////////////////////////////////////////////////////////////////////////////////////////////////
struct ThspMessage
{
    ThspMessage() : packetType(THSP_PACKET_UNKNOWN), command(0), dataSize(0)  {}
    ThspMessage( EPacketType type, uint8_t code) : packetType(type), command(code), dataSize(0)  {}

    EPacketType		packetType;
    uint8_t 		command;
    uint16_t 		dataSize;
    uint8_t 		data[THSP_MAX_MESSAGE_SIZE];
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//// class ThspPacket
//////////////////////////////////////////////////////////////////////////////////////////////////////
//struct ThspPacket
//{
//	EPacketType		packetType;
//    int8_t 			command;
//    uint16_t 		dataSize;
//	SBuf 			sbuf;
//    uint8_t 		checksum1;
//};


#endif // __IO_THSP_PACKET_HPP__
