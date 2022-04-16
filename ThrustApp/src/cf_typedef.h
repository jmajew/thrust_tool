#ifndef __CF_TYPEDEF_H__
#define __CF_TYPEDEF_H__

#include <array>

#include "sysdecl.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
namespace CFstd {

typedef float		Float32;
typedef double		Float64;


typedef uint8_t		UInt8;
typedef int8_t		SInt8;

typedef uint16_t	UInt16;
typedef int16_t		SInt16;

typedef uint32_t	UInt32;
typedef int32_t		SInt32;

typedef uint64_t	UInt64;
typedef int64_t		SInt64;


typedef size_t		TSize;
typedef UInt32		TIndex;
typedef UInt32		UInt;
typedef SInt32		SInt;


typedef TSize	TDimension;

enum
{
	DIM_0D		= 0,
	DIM_1D		= 1,
	DIM_2D		= 2,
	DIM_3D		= 3,
	DIM_4D		= 4
};

template <class RET_TYPE, class ARG_TYPE>
constexpr RET_TYPE construct( const ARG_TYPE& v)
{
	if constexpr ( std::is_class<RET_TYPE>::value )
		return RET_TYPE({v});		//return RET_TYPE({ construct<typename RET_TYPE::TYPE>( v ) });
	else 
		return static_cast<RET_TYPE>(v);
}
 

} // end of namespace CFstd
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//


#endif // __CF_TYPEDEF_H__

