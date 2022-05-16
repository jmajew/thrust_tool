/*
 * thsp_processor.hpp
 *
 *  Created on: 10.03.2019
 *      Author: jmajew
 */

#ifndef __IO_THSP_PROCESSOR_HPP__
#define __IO_THSP_PROCESSOR_HPP__

#include "system/sysdecl.hpp"
#include "util/util.hpp"

#include "io/thsp_packet.hpp"


class Master;

// return positive for ACK, negative on error, zero for no reply
enum ETHSerialResult
{
    THSP_RESULT_NO_REPLY = 2,
    THSP_RESULT_ACK = 1,
    THSP_RESULT_OK = 0,
    THSP_RESULT_ERROR = -1,
    THSP_RESULT_CMD_UNKNOWN = -2,   // don't know how to process command, try another handler
};


enum ETHOutcome
{
	ETH_OUT_UNKNOWN	= 0,
	ETH_OUT_OK,
	ETH_OUT_ERROR
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThspProcessor
////////////////////////////////////////////////////////////////////////////////////////////////////
class ThspProcessor
{
public:
	ThspProcessor( Master *pmst) : mpMaster(pmst)		{}

	ETHSerialResult	ProcessCommand( uint8_t command, SBuf& sbufin, SBuf& sbufout);

private:
	Master *mpMaster;
};



#endif // __IO_THSP_PROCESSOR_HPP__
