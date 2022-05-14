/*
 * sysdecl.cpp
 *
 *  Created on: 17.10.2018
 *      Author: jmajewski
 */

#include "system/sysdecl.hpp"


const char* const sgitRev 		= __REVISION__;
const char* const sbuildDate 	= __DATE__;
const char* const sbuildTime 	= __TIME__;


//===========================================================================
// heap
extern uint8_t __heap_base__;
extern uint8_t __heap_end__;

static size_t heap_size = &__heap_end__ - &__heap_base__;

//static void* ptr = chCoreAllocFromTop( HEAP_STD_SIZE, CH_HEAP_ALIGNMENT, 0);
static char ptr[HEAP_STD_SIZE]
			//__attribute__((section(".data")))
			__attribute__ ((aligned (CH_HEAP_ALIGNMENT)));

chibi::Heap heap( ptr, HEAP_STD_SIZE);



//===========================================================================

void	InitSystem( void)
{
	initErrorLog();
}



#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================
#ifndef SIMULATOR

// __attribute__((used))
// void __cxa_pure_virtual(void)
// {
	// osalSysHalt("__cxa_pure_virtual called");
	// while (TRUE);
// }



// stub needed for  __libc_fini_array
// should be placed in chibios -> syscalls_cpp.cpp
__attribute__((used))
void _fini( void)
{
   osalSysHalt("_fini called");
   while(TRUE);
}

#endif // SIMULATOR
//===========================================================================

#ifdef __cplusplus
}
#endif
