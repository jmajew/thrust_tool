/*
 * io.hpp
 *
 *  Created on: Mar 11, 2019
 *      Author: jmajew
 */

#ifndef __IO_IO_HPP__
#define __IO_IO_HPP__

#include "system/sysdecl.hpp"
#include "system/setup.h"
#include "io/concbaseasyncchannel.hpp"
#include "io/cstream.hpp"


//#include "io/usb.hpp"



using namespace cio;

extern cio::costream	ccout;


////////////////////////////////////////////////////////////////////////////////////////////////////
// class IoMaster
////////////////////////////////////////////////////////////////////////////////////////////////////
class IoMaster
{
public:
	IoMaster( BaseAsynchronousChannel* po, BaseAsynchronousChannel* ps)
		: mOutput(po), mSerialComm(ps)		{}
	
	void	Start();
	
	ConcBaseAsyncChannel&	rOutput() { return mOutput; }
	ConcBaseAsyncChannel&	rSerial() { return mSerialComm; }

private:
//	IoUSB	mUSB;
	
//	BaseAsynchronousChannel*	mpIOOutput;
//	BaseAsynchronousChannel*	mpIOSerialComm;

	ConcBaseAsyncChannel	mOutput;
	ConcBaseAsyncChannel	mSerialComm;
};


//extern IoMaster ioMaster;


#endif // __IO_IO_HPP__
