/*
 * sysdbg.hpp
 *
 *  Created on: 23.10.2018
 *      Author: jmajew
 */

#ifndef __SYSTEM_SYSDBG_H__
#define __SYSTEM_SYSDBG_H__


//==================================================================================================
// ASSERT, CHECK, THROW_INTERNAL defs

// defines name of header or implementation file
#define THIS_FILE __FILE__  


#if ( USE_ERROR_LOG == 1 )

#define ERR_BUFFER_SIZE		128

extern char err_buffer[ERR_BUFFER_SIZE];

#endif // ( USE_ERROR_LOG == 1 )


inline void	initErrorLog()
{
#if ( USE_ERROR_LOG == 1 )
	strncpy( err_buffer, "OK", ERR_BUFFER_SIZE);
#endif
}


#ifdef _DEBUG

int		dbg_printf( const char *fmt, ...);
void 	dbg_puts(const char *s );

#else // _DEBUG

int		dbg_printf( const char *fmt, ...)
{
	(void*)fmt;
}

void 	dbg_puts(const char *s )
{
	(void*)s;
}

#endif // _DEBUG


inline void funcAssert( bool c, const char* s, const char* file, int line)
{
	(void)s;
	(void)file;
	(void)line;

	if (!(c))
	{
#if ( USE_ERROR_LOG == 1 )
		chsnprintf( err_buffer, ERR_BUFFER_SIZE, "%s: file %s, line %d\r\n", s, file, line);
		dbg_puts( err_buffer);
#endif
		chSysHalt(__func__);
	}
}


#ifdef _DEBUG

	#define THROW_INTERNAL(f)                                       				\
		{ funcAssert( false, "internal error: '" #f "'", THIS_FILE, __LINE__ ); }

	#define ASSERT(c)                                               				\
		{ funcAssert( (c), "assertion failed: '" #c "'", THIS_FILE, __LINE__ ); }

#else //_DEBUG

	#define THROW_INTERNAL(f)	{}

	#define ASSERT(f)       	{}

#endif // _DEBUG


#ifdef USE_HFGEOM_SIZE_CHECKING

	#define CHECK(c)                                       						\
		{ funcAssert( (c), "check failed: '" #c "'", THIS_FILE, __LINE__ ); }

#else
	#define CHECK(c)    {}
#endif


#endif // __SYSTEM_SYSDBG_H__
