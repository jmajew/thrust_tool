/*
 * th_serial.hpp
 *
 *  Created on: 18 May 2022
 *      Author: jmajew
 */

#ifndef __TH_ACCEL_HPP__
#define __TH_ACCEL_HPP__

#include "system/sysdecl.hpp"
#include "common.hpp"
#include "util/util.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThAccel
////////////////////////////////////////////////////////////////////////////////////////////////////
//#define STACK_SIZE 256
#define STACK_SIZE 256
class AccelThread : public chibi::BaseStaticThread<STACK_SIZE>
{
public:
	AccelThread()
		: chibi::BaseStaticThread<STACK_SIZE>()		{}

	virtual ~AccelThread()				{}

protected:

	void 	main() override;

private:
};


#undef STACK_SIZE


#endif // __TH_ACCEL_HPP__
