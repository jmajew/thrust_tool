/*
 * stats.hpp
 *
 *  Created on: Mar 10, 2019
 *      Author: jmajew
 */

#ifndef __UTIL_STATS_HPP__
#define __UTIL_STATS_HPP__

#include "system/sysdecl.hpp"



template <typename TYPE>
class Result
{
public:

private:
	uint16_t	mNSample;
	TYPE		mMean;
	TYPE		mDeviation;
};


#endif // __UTIL_STATS_HPP__
