/*
 * stat_value.hpp
 *
 *  Created on: 04.07.2019
 *      Author: jmajew
 */



#ifndef __UTIL_STAT_VALUE_HPP__
#define __UTIL_STAT_VALUE_HPP__

#include <math.h>

#ifdef WIN32
	#include "sysdecl.h"
#else
	#include "system/sysdecl.hpp"
	#include "util.hpp"
#endif 




////////////////////////////////////////////////////////////////////////////////////////////////////
// class StatValue
// T0 - count; T1 - value; T2 - value*count; T4 - value^2*count
////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T0, typename T1, typename T2, typename T4>
class StatValue
{
public:
	using TValue = T1;

	StatValue( T0 m=0, T2 sum=0, T4 sumsq=0 ) : n(m), sx(sum), sx2(sumsq)		{}
	StatValue( T1 val ) : n(1), sx( static_cast<T2>(val) ), sx2( static_cast<T4>(val) * static_cast<T4>(val) )		{}

	StatValue( const StatValue& ) = default;

	void Reset()
	{
		this->n = 0;
		this->sx = 0;
		this->sx2 = 0;
	}

	void Accumulate( T2 val )
	{
		this->n += 1;
		this->sx += val;
		this->sx2 += val*val;
	}

	void Accumulate( StatValue& v )
	{
		this->n += v.n;
		this->sx += v.sx;
		this->sx2 += v.sx2;
	}

	T0	Count() const					{ return n; }

	T1	Mean() const					{ return static_cast<T1>(sx / n); }

	T2	Variance() const
	{
		T4 mean = this->Mean();
		return static_cast<T2>(sx2 / n - mean*mean);
	}

	T1	Deviation() const				{ return static_cast<T1>( sqrt( this->Variance() ) ); }

	constexpr uint8_t	Size() const 	{ return (sizeof( T0 ) + sizeof( T2 ) + sizeof( T4 )); }

public:
//private:
	T0	n;		// sum_i 1
	T2	sx;		// sum_i x_i
	T4	sx2;	// sum_i x_i^2
};


typedef StatValue< uint16_t, uint16_t, uint32_t, uint64_t>	StatValueU12;
typedef StatValue< uint16_t, uint16_t, uint32_t, uint64_t>	StatValueU16;
typedef StatValue< uint16_t, int32_t, int64_t, int64_t>		StatValueS24;
typedef StatValue< uint16_t, double, double, double>		StatValueDbl;



#endif // __UTIL_STAT_VALUE_HPP__
