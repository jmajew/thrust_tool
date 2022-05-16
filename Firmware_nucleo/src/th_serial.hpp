/*
 * th_serial.hpp
 *
 *  Created on: 20 Feb 2021
 *      Author: jmajew
 */

#ifndef __TH_SERIAL_HPP__
#define __TH_SERIAL_HPP__

#include "system/sysdecl.hpp"
#include "common.hpp"
#include "util/util.hpp"

#include "io/thsp_port.hpp"
#include "io/thsp_processor.hpp"


class Master;

enum class ESerialMode
{
	Unknown = 0,
	Command,
	Data_stream,
	Terminal	// ???
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThSerial
////////////////////////////////////////////////////////////////////////////////////////////////////
//#define STACK_SIZE 256
#define STACK_SIZE 378 //256
class SerialThread : public chibi::BaseStaticThread<STACK_SIZE>
{
public:
	SerialThread( ThspPort*	ptr=nullptr)
		: chibi::BaseStaticThread<STACK_SIZE>(),
		  mMode(ESerialMode::Unknown),
		  mpPort(ptr)		{}

	virtual ~SerialThread()				{}

	void	SetMode( ESerialMode mode)	{ mMode = mode; }

//	void	Init( ThspPort*	ptr)	{ mpPort = ptr; }

protected:

	void	readMessage();
	void	execMessage();
	void	sendMessage();

	void 	main() override;

private:
	ESerialMode	mMode;
	ThspPort*	mpPort;
};


#undef STACK_SIZE


#endif // __TH_SERIAL_HPP__
