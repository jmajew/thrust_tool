/*
 * sysdbg.cpp
 *
 *  Created on: 25.10.2018
 *      Author: jmajewski
 */

#include "system/sysdecl.hpp"


#if ( USE_ERROR_LOG == 1 )
char err_buffer[ERR_BUFFER_SIZE] = {0};
#endif

#ifdef _DEBUG

#define DBG_BUFFER_LEN 	128
char _dbg_buffer[DBG_BUFFER_LEN] = {0};

chibi::Mutex _dbg_mutex;

// TODO :: should be protected by mutex
int dbg_printf( const char *fmt, ...)
{
	chibi::MutexLocker mxlock( _dbg_mutex );

	va_list ap;

	va_start(ap, fmt);
	chvsnprintf( _dbg_buffer, DBG_BUFFER_LEN, fmt, ap );
	va_end(ap);

	int count = 0;
	while ( _dbg_buffer[count] != '\0' && count < DBG_BUFFER_LEN)
		ITM_SendChar(_dbg_buffer[ count++ ]);

	return count;
}

void dbg_puts(const char *s )
{
	chibi::MutexLocker mxlock( _dbg_mutex );

//	osalSysLock();
    while (*s)
    	ITM_SendChar(*s++);
//    osalSysUnlock();
}

#else // _DEBUG

#endif // _DEBUG




