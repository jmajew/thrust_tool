/*
 * util.hpp
 *
 *  Created on: 17.10.2018
 *      Author: jmajewski
 */

#ifndef __UTIL_UTIL_HPP__
#define __UTIL_UTIL_HPP__

#include <type_traits>

#include "system/sysdecl.hpp"


#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#define CLAMP( x, min, max)   ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)) )

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
#define ARRAYEND(x) (&(x)[ARRAYLEN(x)])


//////////////////////////////////////////////////////////////////////
// constexpr powint - base ^ exp
//////////////////////////////////////////////////////////////////////
template <class T>
constexpr T powint(T base, int exp) noexcept
{
	return (exp == 0 ? T(1) : base * powint( base, exp - 1));
}


//////////////////////////////////////////////////////////////////////
// constexpr constex_sqrt - square root of Float64
//////////////////////////////////////////////////////////////////////

template <class T>
T constexpr sqrtNewton( T x, T curr, T prev)
{
	return curr == prev	? curr : sqrtNewton(x, 0.5 * (curr + x / curr), curr);
}

template <class T>
T constexpr constex_sqrt( T x)
{
	return x >= 0 && x < std::numeric_limits<T>::infinity()
		? sqrtNewton( x, x, 0)
		: std::numeric_limits<T>::quiet_NaN();
}



//////////////////////////////////////////////////////////////////////
// isqrt - sqrt for integer types
//////////////////////////////////////////////////////////////////////
// FIXME ::
template <class T>
inline T isqrt( T a)
{
	static_assert( std::is_integral_v<T> );
	//static_assert( std::is_signed<T>::value );

	using TYPE = std::make_signed_t<T>;

	if ( a <= 0 )
		return 0;

	TYPE tmp = a;
	int idig = 0;
	do
	{
		++idig;
		tmp >>= 1;
	}
	while ( tmp);

	TYPE guess = a >> ( idig >> 1);

	do
	{
		tmp = (a / guess - guess) >> 1;
		guess += tmp;
	}
	while ( std::abs(tmp) > TYPE(1) );

	if ( guess*guess > a)
		--guess;

	return static_cast<T>( guess );
}

//////////////////////////////////////////////////////////////////////
// shift in data using line or array of lines
//////////////////////////////////////////////////////////////////////

// bitorder
#define MSBFIRST	1
#define LSBFIRST	2

uint8_t shiftIn( ioline_t lineSCK, ioline_t lineDATA, uint8_t bitOrder);

void 	shiftInArray( ioline_t lineSCK, const ioline_t* tabLineDATA, uint8_t* tabOut, uint8_t length,
     	              uint8_t bitOrder);



#endif // __UTIL_UTIL_HPP__
