/*
 * thsp_port.hpp
 *
 *  Created on: 10.03.2019
 *      Author: jmajew
 */

#ifndef __IO_THSP_PORT_HPP__
#define __IO_THSP_PORT_HPP__

#include "system/sysdecl.hpp"
#include "util/util.hpp"

#include "io/thsp_handler.hpp"
#include "io/thsp_processor.hpp"

//class ThspProcessor;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThspPort
////////////////////////////////////////////////////////////////////////////////////////////////////
class ThspPort : public ThspHandler
{
public:
	ThspPort( BaseAsynchronousChannel* psd, ThspProcessor* ptr = nullptr )
		: mpSD(psd), mpProc(ptr)		{}

	BaseAsynchronousChannel*	pSD()						{ return mpSD;}

	void	SetProcessor( ThspProcessor* ptr)				{ mpProc = ptr; }

	ETHSerialResult	ProcessReceivedCommand( ThspMessage *command, ThspMessage *reply );

private:
	BaseAsynchronousChannel*	mpSD;
	ThspProcessor*				mpProc;
};


#endif // __IO_THSP_PORT_HPP__
