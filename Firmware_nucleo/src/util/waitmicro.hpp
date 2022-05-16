/*
 * waitmicro.hpp
 *
 *  Created on: Mar 11, 2019
 *      Author: jmajew
 */

#ifndef UTIL_WAITMICRO_HPP_
#define UTIL_WAITMICRO_HPP_

#include "system/sysdecl.hpp"

void 	gptInitTimer( void);
void 	gptWaitMicro( uint16_t time);


#endif /* UTIL_WAITMICRO_HPP_ */
