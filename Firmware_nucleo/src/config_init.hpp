/*
 * config_init.hpp
 *
 *  Created on: 04.05.2021
 *      Author: jmajew
 */

#ifndef __CONFIG_INIT_HPP__
#define __CONFIG_INIT_HPP__


#include "config.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ConfigInit
////////////////////////////////////////////////////////////////////////////////////////////////////
class ConfigInit
{
public:
	static void		Init( Config* pcfg );
	static void		SetDefault( Config* pcfg );
};


#endif // __CONFIG_INIT_HPP__
