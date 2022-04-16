/*
 * motor.hpp
 *
 *  Created on: 25.03.2019
 *      Author: jmajew
 */

#ifndef __DRIVER_MOTOR_HPP__
#define __DRIVER_MOTOR_HPP__

#include "system/sysdecl.hpp"
#include "config.hpp"

#define MOTOR_NULL_COMMAND		1000
#define MOTOR_HALF_COMMAND		1500
#define MOTOR_MAX_COMMAND		2000


enum EMotorState
{
	MOT_STATE_UNARMED,
	MOT_STATE_STOPED,
	MOT_STATE_RUNNING
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class MotorDev
////////////////////////////////////////////////////////////////////////////////////////////////////
class MotorDev
{
public:
	MotorDev( const CfgMotor* pcfg)
		: mpConfig(pcfg), mState(MOT_STATE_UNARMED), mpPWMDriver(nullptr), mpPWMConfig(nullptr),
		  mOutput(MOTOR_NULL_COMMAND)							{}

	void	Init( PWMDriver* pdrv, uint8_t ch);

	void	Start();
	void	Stop();

	void	Arm();
	void	Disarm();
	void	SetOutput( uint32_t command );

	uint32_t	GetOutput();

	EMotorState	GetState()		{ return mState; }

private:
	const CfgMotor*	mpConfig;
	EMotorState		mState;

	PWMDriver*		mpPWMDriver;
	uint8_t			mChannel;
	PWMConfig*		mpPWMConfig;

	uint32_t 		mOutput;
};

#endif // __DRIVER_MOTOR_HPP__
