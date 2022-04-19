/*
 * thsp_protocol.hpp
 *
 *  Created on: 07.03.2019
 *      Author: jmajew
 */

// Definition of the protocol for communicating with thrust measurement tool.
// Inspired by the MSP protocol.

#ifndef __IO_THSP_PROTOCOL_HPP__
#define __IO_THSP_PROTOCOL_HPP__


#define THSP_PROTOCOL_VERSION             	1

#define API_VERSION_MAJOR                   1
#define API_VERSION_MINOR                   0

#define API_VERSION_LENGTH                  2

//------------------------------------------------------------------------------
// system commands 1-64

#define THSP_API_VERSION                	1

#define THSP_SOFT_INFO              		3
#define THSP_BOARD_INFO                  	4
#define THSP_BUILD_INFO                  	5

//------------------------------------------------------------------------------

#define THSP_GET_CH_COUNT					20	// Motor, StG, ADC, RPM, EscTelem

#define THSP_GET_CONFIG_ADC					32
#define THSP_GET_CONFIG_STGAUGE				33
#define THSP_GET_CONFIG_MOTOR				34
#define THSP_GET_CONFIG_RPM					35
#define THSP_GET_CONFIG_CONVERT				36

#define THSP_READ_CONFIG_FROM_FLASH			39	// should force restart of the firmware

#define THSP_GET_DATA_ZERO					40 // TODO :: should be droped

//------------------------------------------------------------------------------

#define THSP_SET_CONFIG_ADC					52
#define THSP_SET_CONFIG_STGAUGE				53
#define THSP_SET_CONFIG_MOTOR				54
#define THSP_SET_CONFIG_RPM					55
#define THSP_SET_CONFIG_CONVERT				56

#define THSP_WRITE_CONFIG_TO_FLASH			59	// should force restart of the firmware

#define THSP_SET_DATA_ZERO					60 // TODO :: should be droped

//------------------------------------------------------------------------------
// commands 65-128?

#define THSP_SHELL_START					65

#define THSP_MEASURE_START					66
#define THSP_MEASURE_STOP					67

#define THSP_CALIBRATE_ZERO_START			68 // TODO :: should be droped
#define THSP_CALIBRATE_ZERO_STOP			69 // TODO :: should be droped

#define THSP_MOTOR_ARM						70
#define THSP_MOTOR_DISARM					71
#define THSP_MOTOR_SET_COMMAND				72
#define THSP_MOTOR_STOP						73

#define THSP_ESCTELEM_START					80
#define THSP_ESCTELEM_STOP					81

#define THSP_RPM_START						85
#define THSP_RPM_STOP						86

#define THSP_DATASTREAM_START				90
#define THSP_DATASTREAM_STOP				91
#define THSP_SEND_DATA						92	// Motor, StG, ADC, RPM, EscTelem

#define THSP_SEND_HEARTBEAT					95

// set/get commands

//------------------------------------------------------------------------------
#define THSP_RESET							127


#endif // __IO_THSP_PROTOCOL_HPP__
