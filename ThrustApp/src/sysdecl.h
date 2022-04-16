#ifndef __SYSDECL_H__
#define __SYSDECL_H__

#include <cstdlib>
#include <cstring>
#include <cinttypes>


#define CLAMP( x, min, max)   ((x) > (max) ? (max) : ((x) < (min) ? (min) : (x)) )

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
#define ARRAYEND(x) (&(x)[ARRAYLEN(x)])


#ifdef _DEBUG
	#ifdef assert
		// if compiler provides a assert macro in "assert.h"
		#define ASSERT assert
	//#else
	//	// assert macro which does not depend on compiler; uses std::exception ExceptAssert
	//	#define ASSERT(f) \
	//	{ \
	//	if (!(f)) \
	//		throw CFstd::ExceptAssert( "assertion failed: '" #f "'", THIS_FILE, __LINE__ ); \
	//	}
	#endif // assert
#else
	#define ASSERT(f) \
	{ \
	}
#endif // _DEBUG
 

#endif // __SYSDECL_H__

