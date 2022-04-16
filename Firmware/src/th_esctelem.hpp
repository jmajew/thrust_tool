/*
 * th_serial.hpp
 *
 *  Created on: 20 Feb 2021
 *      Author: jmajew
 */

//	One transmission will have 10 8-bit bytes sent with 115200 baud and 3.6V.
//	Byte 0: Temperature
//	Byte 1: Voltage high byte
//	Byte 2: Voltage low byte
//	Byte 3: Current high byte
//	Byte 4: Current low byte
//	Byte 5: Consumption high byte
//	Byte 6: Consumption low byte
//	Byte 7: Rpm high byte
//	Byte 8: Rpm low byte
//	Byte 9: 8-bit CRC
//
//	Converting the received values to standard units
//
//	int8_t Temperature = Temperature in 1°C
//	uint16_t Voltage = Volt *100 so 1000 are 10.00V
//	uint16_t Current = Ampere * 100 so 1000 are 10.00A
//	uint16_t Consumption = Consumption in 1mAh
//	uint16_t ERpm = Electrical Rpm /100 so 100 are 10000 Erpm

#ifndef __TH_ESC_TELEM_HPP__
#define __TH_ESC_TELEM_HPP__

#include "system/sysdecl.hpp"
#include "common.hpp"
#include "util/util.hpp"


class Master;
class Data;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThSerial
////////////////////////////////////////////////////////////////////////////////////////////////////
#define STACK_SIZE 378
class EscTelemThread : public chibi::BaseStaticThread<STACK_SIZE>
{
public:
	EscTelemThread( BaseAsynchronousChannel* ptr=nullptr)
		: chibi::BaseStaticThread<STACK_SIZE>(),
		  mpData(nullptr),
		  mpSD(ptr)

	{}

	void	SetData( Data* pdat)	{ mpData = pdat; }

protected:
	bool	processBuffer();

	void 	main() override;

private:
	Data*						mpData;
	BaseAsynchronousChannel*	mpSD;
};


#undef STACK_SIZE


#endif // __TH_ESC_TELEM_HPP__
