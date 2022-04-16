/*
 * sysdecl.hpp
 *
 *  Created on: 17.10.2018
 *      Author: jmajewski
 */

#ifndef __SYSTEM_SYSDECL_HPP__
#define __SYSTEM_SYSDECL_HPP__

// c
#include <cstdlib>
#include <cstring>

// c++
#include <algorithm>
#include <functional>
#include <utility>
#include <limits>
#include <memory>
#include <new>

// chibios
#include "ch.hpp"
#include "hal.h"
#include "chprintf.h"

#include "system/setup.h"


namespace chibi = chibios_rt;


// HACK :: should be removed
#ifndef _DEBUG
#define _DEBUG
#endif

// options
#define USE_STM32F7_DCACHE			0

#define USE_HFGEOM_SIZE_CHECKING	1

#define USE_ERROR_LOG				1

#define USE_VERBOSE_ALLOC			1


#define HEAP_STD_SIZE				32768
//#define HEAP_STD_SIZE				16384



// local
//#include "system/heap_allocator.hpp"
#include "system/sysdbg.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
// preprocessor defs

//--------------------------------------------------------------------------------------------------
//version defs
#define GIT_SHORT_REVISION_LENGTH   7	// lower case hexadecimal digits.
extern const char* const sgitRev;

#define BUILD_DATE_LENGTH			11	// MMM DD YYYY" MMM
extern const char* const sbuildDate;

#define BUILD_TIME_LENGTH			8	// HH:MM:SS
extern const char* const sbuildTime;


#define FC_FIRMWARE_NAME            "QQthrust"
#define FC_VERSION_MAJOR            0
#define FC_VERSION_MINOR            1
#define FC_VERSION_PATCH_LEVEL      0


//==================================================================================================

//__attribute__((section(".ram3")));
//__attribute__((section(".nocache")));


//==================================================================================================
// user defined messages:
#define MSG_COMM_ERROR             (msg_t)-100


//==================================================================================================
#define BUILD_ASSERT(condition) ((void)sizeof(char[1 - 2*!(condition)]))


#ifdef SIMULATOR
	#define ATTR_PACKED
#else
	#define ATTR_PACKED		__attribute__((packed))
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// C/C++ defs


//==================================================================================================
//inline void *operator new	(size_t size) noexcept		{ return chibi::Core::alloc(size); }
//inline void *operator new[]	(size_t size) noexcept		{ return chibi::Core::alloc(size); }


extern chibi::Heap	heap;

inline void* operator new	(size_t sz)  noexcept
{
    void* ptr = reinterpret_cast<void*>( heap.alloc( sz * sizeof( char ) ) );
    dbg_printf( "-- new %d bytes at %p\r\n", sz, ptr);

    return ptr;
}

inline void* operator new[]	(size_t sz)  noexcept
{
	void* ptr = reinterpret_cast<void*>( heap.alloc( sz * sizeof( char ) ) );
    dbg_printf( "-- new [] %d bytes at %p\r\n", sz, ptr);

    return ptr;
}

inline void operator delete( void* ptr) noexcept
{
    dbg_printf( "-- delete at %p\r\n", ptr);
//	ccout << "-- delete at " << ptr << endl;
    heap.free( ptr );
}

inline void operator delete[]( void* ptr) noexcept
{
    dbg_printf( "-- delete [] at %p\r\n", ptr);
//	ccout << "-- delete [] at " << ptr << endl;
    heap.free( ptr );
}

inline void operator delete( void* ptr, size_t sz) noexcept
{
	(void)sz;
    dbg_printf( "-- delete (size) at %p\r\n", ptr);
//	ccout << "-- delete (size) at " << ptr << endl;
    heap.free( ptr );
}

inline void operator delete[]( void* ptr, size_t sz) noexcept
{
	(void)sz;
    dbg_printf( "-- delete [] (size) at %p\r\n", ptr);
//	ccout << "-- delete [] (size) at " << ptr << endl;
    heap.free( ptr );
}



////==================================================================================================
////inline void *operator new	(size_t size) noexcept		{ return chibi::Core::alloc(size); }
////inline void *operator new[]	(size_t size) noexcept		{ return chibi::Core::alloc(size); }
//
//inline void* operator new	(size_t sz)  noexcept
//{
//    void* ptr = reinterpret_cast<void*>( heap_allocator<char>().allocate( sz) );
////    ccout << "-- new " << sz << " bytes at " << ptr << endl;
//
//    return ptr;
//}
//
//inline void* operator new[]	(size_t sz)  noexcept
//{
//	void* ptr = reinterpret_cast<void*>( heap_allo	cator<char>().allocate( sz) );
////    ccout << "-- new [] " << sz << " bytes at " << ptr << endl;
//
//    return ptr;
//}
//
//inline void operator delete( void* ptr) noexcept
//{
////	ccout << "-- delete at " << ptr << endl;
//    heap_allocator<char>().deallocate( (char*)ptr, 0);
//}
//
//inline void operator delete[]( void* ptr) noexcept
//{
////	ccout << "-- delete [] at " << ptr << endl;
//    heap_allocator<char>().deallocate( (char*)ptr, 0);
//}
//
//inline void operator delete( void* ptr, size_t sz) noexcept
//{
//	(void)sz;
////	ccout << "-- delete (size) at " << ptr << endl;
//    heap_allocator<char>().deallocate( (char*)ptr, sz);
//}
//
//inline void operator delete[]( void* ptr, size_t sz) noexcept
//{
//	(void)sz;
////	ccout << "-- delete [] (size) at " << ptr << endl;
//    heap_allocator<char>().deallocate( (char*)ptr, sz);
//}

//==================================================================================================
typedef float		TFloat;
typedef size_t		TSize;
typedef uint8_t		TByte;
typedef int			TInt;
typedef TSize		TDimension;

//==================================================================================================
void	InitSystem( void);

#endif // __SYSTEM_SYSDECL_HPP__
