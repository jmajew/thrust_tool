/*
 * common.cpp
 *
 *  Created on: 16 Feb 2021
 *      Author: jmajew
 */

#include "common.hpp"

chibi::EventSource	evtMeasure;
chibi::EventSource	evtDataRdy;
chibi::EventSource	evtShell;

chibi::EventSource	evtRpm;

//chibi::EventSource	evtMotorCommand;
//chibi::EventSource	evtMotorArm;
//chibi::EventSource	evtMotorDisarm;


chibi::BinarySemaphore	sem(false);

