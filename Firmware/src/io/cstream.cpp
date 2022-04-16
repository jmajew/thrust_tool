/*
 * cstream.cpp
 *
 *  Created on: 26.10.2018
 *      Author: jmajewski
 */

//#include <cstdio>
#include "system/sysdecl.hpp"
#include "io/cstream.hpp"
#include "util/util.hpp"

// #include "io/usb.hpp"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

// USB driver must be initialized BEFORE using 'ccout'
namespace cio
{

//costream	ccout( reinterpret_cast<BaseSequentialStream*>( &SDU1) );
//costream	ccout( IoUSB::pConcBSStream() );


#define FLOAT_PRECISION 9


static char* long_to_string_with_divisor( char *p, long num, unsigned radix, long divisor)
{
	int i;

	long l = num;
	long ll = divisor;
	if ( divisor == 0)
		ll = num;

	char *q = p;
	do
	{
		i = (int) (l % radix);
		i += '0';
		if ( i > '9')
			i += 'A' - '0' - 10;
		*q++ = i;
		l /= radix;
	}
	while ( (ll /= radix) != 0);

	i = (int) (q - p)/2;
	char* r = q--;
	while ( i--)
	{
		char c = *q;
		*q-- = *p;
		*p++ = c;
	}

	return r;
}


char* ftoa( char *p, float num, unsigned long precision)
{
	if ( (precision == 0) || (precision > FLOAT_PRECISION))
		precision = FLOAT_PRECISION;

	if ( num < 0.0f )
	{
		num = -num;
		*p++ = '-';
	}

	int ex = 1;
	char csig = 0;
	if ( num > 1.0e4f )
	{
		csig = '+';
		while ( (num /= 10.0f) > 10.0f)
			++ex;
	}
	else if ( num > 0.0f && num < 0.01f )
	{
		csig = '-';
		while ( (num *= 10.0f) < 1.0f)
			++ex;
	}

	long tmp = (unsigned long) num;
	long nd = 0;
	do
		++nd;
	while ( (tmp/=10) != 0 );

	if ( (long)precision > nd )
		precision -= nd;
	else
		precision = 1;

	long k = precision-1;
	precision = 10;
	while ( k-- )
		precision *=10;

	long l = (long) num;
	p = long_to_string_with_divisor( p, l, 10, 0);
	*p++ = '.';
	l = (long) ((num - l) * precision + 0.5f);
	p = long_to_string_with_divisor( p, abs(l), 10, precision / 10);

	if ( ex > 1 )
	{
		*p++ = 'e';
		*p++ = csig;
		*p++ = ex/10 + '0';
		*p++ = ex%10 + '0';
	}
	*p = 0;

	return p;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class costream
////////////////////////////////////////////////////////////////////////////////////////////////////

int costream::cstream_fprintf( const char *fmt, ...)
{
	va_list ap;

	chibi::MutexLocker mxlock( this->cFile()->rMutex() );

//  mx.lock();
	va_start(ap, fmt);
	int formatted_bytes = chvprintf( this->cFile()->chp(), fmt, ap);
	va_end(ap);
//  mx.unlock();

	return formatted_bytes;
}

costream&	costream::operator << ( const float f )
{
	char buf[CSTREAM_BUFF_SIZE];
	memset( buf, 0, CSTREAM_BUFF_SIZE);
	ftoa( buf, f, this->prec);
	int n = strlen( buf);
	int w = MIN( this->wide, CSTREAM_BUFF_SIZE-1);
	if ( n < w )
	{
		int shift = w - n;
		memmove( buf + shift, buf, n+1);
		memset( buf, ' ', shift);
	}
	return operator << ( buf);

//		int e;
//		if ( (e = cstream_fprintf( this->cFile(), "%s", buf) ) < 0 )
//		// if ( (e = cstream_fprintf( this->cFile(), "%*.*f", this->wide, this->prec, f) ) < 0 )
//			err = e;
//		return *this;
}


} // end namespace cio
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//


