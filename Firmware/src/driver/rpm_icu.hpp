/*
 * rpm_icu.hpp
 *
 *  Created on: Mar 12, 2021
 *      Author: jmajew
 */

#ifndef __DRIVER_RPM_ICU_HPP__
#define __DRIVER_RPM_ICU_HPP__


#include "system/sysdecl.hpp"
#include "config.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
// class MotorDev
////////////////////////////////////////////////////////////////////////////////////////////////////
class RpmDev
{
public:
	RpmDev( const CfgRpm* pcfg)
		: mpConfig(pcfg), mpICUDriver(nullptr), mpICUConfig(nullptr)
	{}

	void	Init( ICUDriver* pdrv, icuchannel_t ch);

	void	Start();
	void	Stop();

	uint32_t	GetOutput();

private:
	const CfgRpm*	mpConfig;

	ICUDriver*		mpICUDriver;
	icuchannel_t	mChannel;
	ICUConfig*		mpICUConfig;
};


#endif // __DRIVER_RPM_ICU_HPP__
