/*
 * motor.cpp
 *
 *  Created on: 25.03.2019
 *      Author: jmajew
 */

#include "driver/motor.hpp"
#include "util/util.hpp"



//// Standard PWM config
//static PWMConfig pwmcfg =
//{
//    84000000,    // PWM clock frequency.
//    1680000,      // Initial PWM period 20ms.
//    NULL, 	  //pwmpcb,
//    {
//    	{PWM_OUTPUT_DISABLED, NULL},
//    	{PWM_OUTPUT_DISABLED, NULL},
//        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
//    	{PWM_OUTPUT_DISABLED, NULL},
//    },
//    0,	// cr2
//    0,	// bdtr
//	0	// dier
//};


// Standard PWM 20ms config
static PWMConfig pwmcfg_20 =
{
    1000000,    // 1MHz PWM clock frequency.
    20000,      // Initial PWM period 20ms.
    NULL, 	  //pwmpcb,
    {
    	{PWM_OUTPUT_DISABLED, NULL},
    	{PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    	{PWM_OUTPUT_DISABLED, NULL},
    },
    0,	// cr2
    0,	// bdtr
	0	// dier
};


// OneShot135 config
static PWMConfig pwmcfg_OneShot125 =
{
    84000000,    // PWM clock frequency.
	42000,      // Initial PWM period 500us.
    NULL, 	  //pwmpcb,
    {
    	{PWM_OUTPUT_DISABLED, NULL},
    	{PWM_OUTPUT_DISABLED, NULL},
        {PWM_OUTPUT_ACTIVE_HIGH, NULL},
    	{PWM_OUTPUT_DISABLED, NULL},
    },
    0,	// cr2
    0,	// bdtr
	0	// dier
};

void MotorDev::Init( PWMDriver* pdrv, uint8_t ch)
{
	mpPWMDriver = pdrv;
	mChannel = ch;
}

void MotorDev::Start()
{
	if ( mpConfig->protocol == MOTPROT_PWM2000 )
	{
		mpPWMConfig = &pwmcfg_20;
	}
	else if ( mpConfig->protocol == MOTPROT_PWM1000 )
	{
		mpPWMConfig = &pwmcfg_20;
		mpPWMConfig->period = 10000;
	}
	else if ( mpConfig->protocol == MOTPROT_ONESHOT125 )
	{
		mpPWMConfig = &pwmcfg_OneShot125;
		mpPWMConfig->period = 42000;
	}
	else if ( mpConfig->protocol == MOTPROT_ONESHOT42 )
	{
		mpPWMConfig = &pwmcfg_OneShot125;
		mpPWMConfig->period = 14000;
	}
	else
	{
		return;
	}

	pwmStart( mpPWMDriver, mpPWMConfig);
//	pwmEnablePeriodicNotification( mpPWMDriver);
	mState = MOT_STATE_UNARMED;

	SetOutput( mpConfig->zeroOutput);
	//SetOutput( MOTOR_NULL_COMMAND);
}

void MotorDev::Stop()
{
	pwmStop( mpPWMDriver);
}

void MotorDev::Arm()
{
	if ( mState == MOT_STATE_UNARMED )
	{
		mState = MOT_STATE_STOPED;
		SetOutput( mpConfig->zeroOutput);
		//SetOutput( MOTOR_NULL_COMMAND);
	}
}

void MotorDev::Disarm()
{
	SetOutput( mpConfig->zeroOutput);
	//SetOutput( MOTOR_NULL_COMMAND);
	mState = MOT_STATE_UNARMED;
}

void MotorDev::SetOutput( uint32_t command )
{
	if ( mState == MOT_STATE_UNARMED )
		return;

	if ( command < mpConfig->minOutput )
	{
		mState = MOT_STATE_STOPED;
		command = mpConfig->zeroOutput;
		//command = MOTOR_NULL_COMMAND;
	}
	else if ( command >= mpConfig->minOutput )
	{
		mState = MOT_STATE_RUNNING;
	}

//	uint32_t c = motor.output = CLAMP( command, config.groupMotor.minOutput, config.groupMotor.maxOutput);
	uint32_t c = mOutput = CLAMP( command, (uint32_t)mpConfig->zeroOutput, (uint32_t)mpConfig->maxOutput);

	uint32_t width = c;
	switch ( mpConfig->protocol )
	{
		default:
		{
			break;
		}

		case MOTPROT_ONESHOT125:
		{
			width *= 21;
			width /= 2;
			break;
		}

		case MOTPROT_ONESHOT42:
		{
			width *= 3528;
			width /= 1000;
			break;
		}
	}

//	c = c * mpConfig->scale;
//	uint32_t rem = c % 10000;
//	c /= 10000;
//	c += rem > 5000 ? 1 : 0 ;

    pwmEnableChannel( mpPWMDriver, mChannel, width);
//    pwmEnableChannelNotification( mpPWMDriver, mChannel);
}

uint32_t MotorDev::GetOutput()
{
	return mpPWMDriver->tim->CCR[mChannel];
}






