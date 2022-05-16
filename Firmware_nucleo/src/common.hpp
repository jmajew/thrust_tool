/*
 * common.hpp
 *
 *  Created on: 16 Feb 2021
 *      Author: jmajew
 */

#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include "system/sysdecl.hpp"



//enum class EMeasureSignal
//{
//	UNKNOWN = 0,
//	CALIBRATE,
//	START,
//	STOP
//};

#define SIG_MEAS_START			0x0001
#define SIG_MEAS_STOP			0x0002
#define SIG_CALIB_ZERO_START	0x0003
#define SIG_CALIB_ZERO_STOP		0x0004
#define SIG_RPM_START			0x0005
#define SIG_RPM_STOP			0x0006

#define SIG_DATARDY				0x8000


extern chibi::EventSource	evtMeasure;
extern chibi::EventSource	evtDataRdy;
extern chibi::EventSource	evtShell;

extern chibi::EventSource	evtRpm;


#endif // __COMMON_HPP__
