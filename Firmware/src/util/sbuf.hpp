/*
 * sbuf.hpp
 *
 *  Created on: Mar 11, 2019
 *      Author: jmajew
 */

#ifndef UTIL_SBUF_HPP_
#define UTIL_SBUF_HPP_

#ifdef WIN32
	#include "sysdecl.h"
#else
	#include "system/sysdecl.hpp"
#endif


//////////////////////////////////////////////////////////////////////
// class SBuf
//////////////////////////////////////////////////////////////////////
class SBuf
{
public:
//	SBuf( uint8_t* ptr = nullptr) : mpBeg(ptr), mpEnd(ptr) {}
	SBuf( uint8_t* ptr = nullptr, uint16_t size=0) : mpBeg(ptr), mpEnd(ptr+size) {}

	void Init( uint8_t* ptr, uint16_t size=0)
	{
		mpBeg = ptr;
		mpEnd = ptr + size;
	}

	uint8_t*	GetPBeg()				{ return mpBeg; }
	void		SetPBeg( uint8_t* ptr)	{ mpBeg = ptr; }

	uint8_t*	GetPEnd()				{ return mpEnd; }
	void		SetPEnd( uint8_t* ptr)	{ mpEnd = ptr; }

	size_t		Length() const			{ return mpEnd - mpBeg; }


	uint8_t ReadU8()
	{
	    return *mpBeg++;
	}

	uint16_t ReadU16()
	{
	    uint16_t ret;
	    ret = this->ReadU8();
	    ret |= this->ReadU8() << 8;
	    return ret;
	}

	uint32_t ReadU32()
	{
	    uint32_t ret;
	    ret  = this->ReadU8();
	    ret |= this->ReadU8() <<  8;
	    ret |= this->ReadU8() << 16;
	    ret |= this->ReadU8() << 24;
	    return ret;
	}

	uint64_t ReadU64()
	{
	    uint64_t ret;
	    ret  = (uint64_t)this->ReadU8();
	    ret |= (uint64_t)this->ReadU8() <<  8;
	    ret |= (uint64_t)this->ReadU8() << 16;
	    ret |= (uint64_t)this->ReadU8() << 24;
	    ret |= (uint64_t)this->ReadU8() << 32;
	    ret |= (uint64_t)this->ReadU8() << 40;
	    ret |= (uint64_t)this->ReadU8() << 48;
	    ret |= (uint64_t)this->ReadU8() << 56;
	    return ret;
	}

	void ReadData( void *data, int len)
	{
	    memcpy( data, mpEnd, len);		// ????
	    mpBeg += len;
	}


	void WriteU8( uint8_t val)
	{
	    *mpEnd++ = val;
	}

	void WriteU16( uint16_t val)
	{
		this->WriteU8( val >> 0);
	    this->WriteU8( val >> 8);
	}

	void WriteU32( uint32_t val)
	{
		this->WriteU8( val >> 0);
		this->WriteU8( val >> 8);
		this->WriteU8( val >> 16);
		this->WriteU8( val >> 24);
	}

	void WriteU64( uint64_t val)
	{
		this->WriteU8( val >> 0);
		this->WriteU8( val >> 8);
		this->WriteU8( val >> 16);
		this->WriteU8( val >> 24);
		this->WriteU8( val >> 32);
		this->WriteU8( val >> 40);
		this->WriteU8( val >> 48);
		this->WriteU8( val >> 56);
	}

	void WriteData( const void *data, size_t len)
	{
	    memcpy( mpEnd, data, len);
	    mpEnd += len;
	}

	void WriteString( const char *str)
	{
	    WriteData( str, strlen(str));
	}

private:
	// debug :: add max size | pbeg0 and pend0
	uint8_t* mpBeg;
	uint8_t* mpEnd;
};

#endif  // UTIL_SBUF_HPP_
