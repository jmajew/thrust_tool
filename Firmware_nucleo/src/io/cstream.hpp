#ifndef __SYSTEM_CSTREAM_HPP__
#define __SYSTEM_CSTREAM_HPP__


#define _CRT_SECURE_NO_WARNINGS

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

#include "io/concbaseasyncchannel.hpp"



#define CSTREAM_BUFF_SIZE	64

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\//\\//\\//\\//\\//\\//\\//\\/
namespace cio
{

class costream;


char* ftoa( char *p, float num, unsigned long precision);


// store function pointer and argument value
template <class Arg>
struct smanip
{	
	smanip( void ( *func)(costream&, Arg), Arg val) : pfun(func), arg(val)		{}

	void ( *pfun)(costream&, Arg);	// the function pointer
	Arg arg;						// the argument value
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class costream : cstream for output
////////////////////////////////////////////////////////////////////////////////////////////////////
class costream
{
public:
	typedef int streamsize;

	friend void	width( costream& cstr, costream::streamsize w);
	friend void	precision( costream& cstr, costream::streamsize p);

public:
//	costream( const char name[], const char attr[]) : err(0), wide(1), prec(3), pfile(nullptr)
//	{
//		pfile = fopen( name, attr);
//	}

	costream( ConcBaseAsyncChannel* f) : err(0), wide(1), prec(3), pfile(f)	{}

	~costream()
	{
		if ( pfile)
			pfile = nullptr;
//			fclose( pfile);
	}

	costream& operator << ( costream& (*pf)(costream&) )
	{
		return pf( *this);
	}

	costream& operator << ( smanip<streamsize> st )
	{
		st.pfun( *this, st.arg);
		return *this;
	}


	// streaming operators
	costream&	operator << ( const char str[] )
	{
		int e; 
		if ( (e = cstream_fprintf( "%s", str) ) < 0 )
			err = e;
		return *this;
	}

	costream&	operator << ( const int d )
	{
		int e; 
		if ( (e = cstream_fprintf( "%*d", this->wide, d) ) < 0 )
			err = e;
		return *this;
	}

	costream&	operator << ( const uint32_t d )
	{
		int e;
		if ( (e = cstream_fprintf( "%*d", this->wide, d) ) < 0 )
			err = e;
		return *this;
	}

	costream&	operator << ( const size_t d )
	{
		int e;
		if ( (e = cstream_fprintf( "%*d", this->wide, d) ) < 0 )
			err = e;
		return *this;
	}

	costream&	operator << ( const char c )
	{
		int e; 
		if ( (e = cstream_fprintf( "%*c", this->wide, c) ) < 0 )
			err = e;
		return *this;
	}

	costream&	operator << ( const bool b )
	{
		int e;
		if ( b )
			e = cstream_fprintf( "true");
		else
			e = cstream_fprintf( "false");

		if ( e < 0 )
			err = e;
		return *this;
	}

	costream&	operator << ( const void* p )
	{
		int e;
		if ( (e = cstream_fprintf( "0x%*x", this->wide,  p) ) < 0 )
			err = e;
		return *this;
	}

	costream&	operator << ( const float f );


	ConcBaseAsyncChannel*		cFile()			{ return pfile; }
	//streamsize	cPrec() const	{ return prec; }
	//streamsize	cWidth() const	{ return wide; }

	int			Error() const	{ return err; }

	friend costream&	endl( costream& cstr);

private:
	int cstream_fprintf( const char *fmt, ...);


private:
	int						err;
	streamsize				wide;	// field width
	streamsize				prec;	// field precision
	ConcBaseAsyncChannel*	pfile;
};
////////////////////////////////////////////////////////////////////////////////////////////////////


// setting width
inline void	width( costream& cstr, costream::streamsize w)
{
	cstr.wide = w;
}

inline smanip<costream::streamsize>	setw( costream::streamsize w)
{
	return smanip<costream::streamsize>( width, w);
}

// setting precision
inline void	precision( costream& cstr, costream::streamsize p)
{
	cstr.prec = p;
}

inline smanip<costream::streamsize>	setprecision( costream::streamsize p)
{
	return smanip<costream::streamsize>( precision, p);
}


// end of line function
inline costream&	endl( costream& cstr)
{
	cstr.cstream_fprintf( "\r\n");
//	fflush( cstr.cFile() );
	return ( cstr);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// std cstream out
// extern costream	ccout;


} // end namespace cio
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\//\\//\\//\\//\\//\\//\\//\\/

#endif // __SYSTEM_CSTREAM_HPP__
