/*
 * th_measure.hpp
 *
 *  Created on: Feb 19, 2021
 *      Author: jmajew
 */

#ifndef __TH_MEASURE_HPP__
#define __TH_MEASURE_HPP__

#include "system/sysdecl.hpp"
#include "common.hpp"

#include "data.hpp"
#include "config.hpp"
#include "driver/hx711.hpp"
#include "driver/adc.hpp"
#include "driver/rpm_icu.hpp"





#define STACK_SIZE 378 //256
class MeasureThread: public chibi::BaseStaticThread<STACK_SIZE>
{
public:
	MeasureThread( HX711Dev* phx, ADConvertDev* padc, RpmDev* prpm)
			: chibi::BaseStaticThread<STACK_SIZE>(),
			  mpData(nullptr),
			  mpHX711Dev(phx), mpADCDev(padc), mpRPMDev(prpm)
	{}

	void	SetData( Data* pdat)	{ mpData = pdat; }

protected:
	void main( void) override;

private:
	Data*			mpData;
	HX711Dev*		mpHX711Dev;
	ADConvertDev*	mpADCDev;
	RpmDev*			mpRPMDev;
};


#undef STACK_SIZE

#endif // __TH_MEASURE_HPP__
