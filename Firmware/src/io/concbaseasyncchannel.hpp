/*
 * concbaseseqstream.hpp
 *
 *  Created on: 06.11.2018
 *      Author: jmajewski
 */

#ifndef __IO_CONCASYNCCHANNEL_HPP__
#define __IO_CONCASYNCCHANNEL_HPP__

#include "system/sysdecl.hpp"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ConcBaseSequentialStream - BaseSequentialStream suplemented with mutex
////////////////////////////////////////////////////////////////////////////////////////////////////
class ConcBaseAsyncChannel
{
public:
	ConcBaseAsyncChannel( BaseAsynchronousChannel*	bachp) : mpBACh(bachp)	{}

	BaseSequentialStream*		chp()		{ return (BaseSequentialStream*)mpBACh; }
	BaseAsynchronousChannel*	bachp()		{ return mpBACh; }
	chibi::Mutex&				rMutex()	{ return mMutex; }

private:
	BaseAsynchronousChannel*	mpBACh;
	chibi::Mutex				mMutex;
};



#endif // __IO_CONCASYNCCHANNEL_HPP__
