#ifndef __CF_GVECTOR_H__
#define __CF_GVECTOR_H__


#include "cf_typedef.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
namespace CFGeomTools {

using namespace CFstd;


template <class TELEM, TSize MAX_SIZE>
class GVector;

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> operator *( const TELEM&,  const GVector<TELEM,MAX_SIZE>&);

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> operator *( const GVector<TELEM,MAX_SIZE>&, const TELEM&);

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> operator /( const GVector<TELEM,MAX_SIZE>&, const TELEM&);

template <class TELEM, TSize MAX_SIZE>
TELEM	operator *( const GVector<TELEM,MAX_SIZE>&, const GVector<TELEM,MAX_SIZE>&);   // dot product

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> operator +( const GVector<TELEM,MAX_SIZE>&, const GVector<TELEM,MAX_SIZE>&);

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> operator -( const GVector<TELEM,MAX_SIZE>&, const GVector<TELEM,MAX_SIZE>&);

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> operator -( const GVector<TELEM,MAX_SIZE>&);

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> Minimum( const GVector<TELEM,MAX_SIZE>& vec1,  const GVector<TELEM,MAX_SIZE>& vec2 );

template <class TELEM, TSize MAX_SIZE>
GVector<TELEM,MAX_SIZE> Maximum( const GVector<TELEM,MAX_SIZE>& vec1,  const GVector<TELEM,MAX_SIZE>& vec2 );



//////////////////////////////////////////////////////////////////////
//	class GVector
//////////////////////////////////////////////////////////////////////
template <class TELEM, TSize MAX_SIZE>
class GVector
{
public:
	enum { SIZE = MAX_SIZE };

	using TYPE	= TELEM;

	//////////////////////////////////////////////////////////////////
	// constructors

	constexpr GVector()
	{
		mtab.fill( construct<TELEM>(0) );
		//for ( TSize i=0; i<MAX_SIZE; mtab[i++]=TELEM{0} );
	}

	//GVector( const TSize& x)
	//{
	//	THROW_INTERNAL( "GVector( TSize) constr called");
	//}

	explicit constexpr GVector( const std::vector<TELEM>& tab)
	{
		std::fill( std::copy( tab.begin(), tab.end(), mtab.begin() ), mtab.end(), construct<TELEM>(0) );
	}


	//GVector( std::initializer_list<TELEM> args)			// TODO :: TEST THIS !!!!!!!!!!!!!!!!
	//{
	//	ASSERT( args.size() <= SIZE );
	//	
	//	if ( args.size() == 1 )
	//	{
	//		mtab.fill( *args.begin() );
	//	}
	//	else
	//	{
	//		//mtab.fill( TELEM{0} );
	//		//std::copy( args.begin(), args.end(), mtab.begin() );
	//		std::fill( std::copy( args.begin(), args.end(), mtab.begin() ), mtab.end(), construct<TELEM>(0) );
	//	}
	//}

	template <class T, TSize N>
	explicit constexpr GVector( const T(&args)[N] )
	{
		static_assert( N <= SIZE, "Too many elements in GVector ctor.");

		if constexpr ( N == 1 )
		{
			mtab.fill( construct<TELEM>( args[0] ) );
		}
		else
		{
			std::fill( std::copy( args, args+N, mtab.begin() ), mtab.end(), construct<TELEM>(0) );
		}
	}

	template <class T> 
	explicit constexpr GVector( const GVector<T,MAX_SIZE> &vec)
	{
		for ( TSize i=0; i<MAX_SIZE; ++i)
			mtab[i] = TELEM( vec.cX(i) );
	}

	template <TSize N> 
	explicit constexpr GVector( const GVector<TELEM,N> &vec)
	{
		const TSize n = N<MAX_SIZE ? N : MAX_SIZE;

		for ( TSize i=0; i<n; ++i)
			mtab[i] = vec.cX(i);
		for ( TSize i=n; i<MAX_SIZE; ++i)
			mtab[i] = 0;
	}

	//GVector( const GVector<TELEM,MAX_SIZE+1> &vct, TSize idim)
	//{
	//	TELEM tab[MAX_SIZE+1];
	//	for ( TSize i=0; i<=MAX_SIZE; ++i)
	//		tab[i] = vct.cX(i);

	//	rotate( tab, tab+idim, tab+MAX_SIZE+1 );

	//	for ( TSize i=1; i<=MAX_SIZE; ++i)
	//			rX(i-1) = tab[i];
	//}


	constexpr void Init( const TELEM& d)
	{
		mtab.fill(d);
	}

	void	Resize( TSize nrows)		{ ASSERT( nrows == MAX_SIZE); } // needed for compatibility


	// insert c at new pos idim
	GVector<TELEM,MAX_SIZE+1> ElevateDim( const TELEM& c, TSize idim) const
	{
		GVector<TELEM,MAX_SIZE+1> vct;

		for ( TSize i=0; i<MAX_SIZE; ++i)
			vct.rX(i) = this->cX(i);
		vct.rX(MAX_SIZE) = c;

		std::rotate( &(vct.rX()), &(vct.rX())+MAX_SIZE-idim, &(vct.rX())+MAX_SIZE+1 );

		return vct;
	}

	// insert c at the end of the new vect
	GVector<TELEM,MAX_SIZE+1> ElevateDim( const TELEM& c) const
	{
		GVector<TELEM,MAX_SIZE+1> vct;
		for ( TSize i=0; i<MAX_SIZE; ++i)
			vct.rX(i) = this->cX(i);
		vct.rX(MAX_SIZE) = c;

		return vct;
	}

	// drop elem at pos idim
	GVector<TELEM,MAX_SIZE-1> RestrictDim( TSize idim)
	{
		GVector<TELEM,MAX_SIZE-1> vct;
		idim = idim>=MAX_SIZE ? idim=MAX_SIZE-1 : idim;

		for ( TSize i=0; i<idim; ++i)
				vct.rX(i) = this->cX(i);
		for ( TSize i=idim+1; i<MAX_SIZE; ++i)
				vct.rX(i-1) = this->cX(i);

		return vct;
	}

	GVector<TELEM,MAX_SIZE-1> RestrictDim()
	{
		GVector<TELEM,MAX_SIZE-1> vct;
		for ( TSize i=0; i<MAX_SIZE-1; ++i)
			vct.rX(i) = this->cX(i);

		return vct;
	}

    //////////////////////////////////////////////////////////////////
	// declaration of friend two argument operators
	friend GVector		operator *<>( const TELEM&,  const GVector&);
	friend GVector		operator *<>( const GVector&, const TELEM&);
	friend GVector		operator /<>( const GVector&, const TELEM&);
	friend TELEM		operator *<>( const GVector&, const GVector&);   // dot product
	friend GVector		operator +<>( const GVector&, const GVector&);
	friend GVector		operator -<>( const GVector&, const GVector&);
	friend GVector		operator -<>( const GVector&);

	friend GVector		Minimum<>( const GVector&, const GVector& );
	friend GVector		Maximum<>( const GVector&, const GVector& );


    //////////////////////////////////////////////////////////////////
	// one argument operators
	GVector<TELEM,MAX_SIZE>&	operator  =( const GVector<TELEM,MAX_SIZE> &vec);
	GVector<TELEM,MAX_SIZE>&	operator +=( const GVector<TELEM,MAX_SIZE>&);
	GVector<TELEM,MAX_SIZE>&	operator -=( const GVector<TELEM,MAX_SIZE>&);
	GVector<TELEM,MAX_SIZE>&	operator *=( const TELEM&);
	GVector<TELEM,MAX_SIZE>&	operator /=( const TELEM&);

	TELEM						Length() const	{ return sqrt( (*this) * (*this) ); }
	GVector<TELEM,MAX_SIZE>		Versor() const	{ return (*this) / Length(); }

	constexpr TSize			NRows() const	{ return MAX_SIZE;}
	constexpr TSize			Size() const	{ return MAX_SIZE;}

	const TELEM&	operator []( TSize i) const	{ return mtab[i];}
	TELEM&			operator []( TSize i)		{ return mtab[i];}

	const TELEM&	operator ()( TSize i) const	{ return mtab[i];}
	TELEM&			operator ()( TSize i)		{ return mtab[i];}

	const TELEM&	cX( TSize i) const			{ return mtab[i];}
	TELEM&			rX( TSize i)				{ return mtab[i];}

	const TELEM&	cX() const		{ return mtab[0];}
	const TELEM&	cY() const		{ ASSERT(MAX_SIZE>1); return mtab[1];}
	const TELEM&	cZ() const		{ ASSERT(MAX_SIZE>2); return mtab[2];}
	const TELEM&	cW() const		{ ASSERT(MAX_SIZE>3); return mtab[3];}

	TELEM&			rX()			{ return mtab[0];}
	TELEM&			rY()			{ ASSERT(MAX_SIZE>1); return mtab[1];}
	TELEM&			rZ()			{ ASSERT(MAX_SIZE>2); return mtab[2];}
	TELEM&			rW()			{ ASSERT(MAX_SIZE>3); return mtab[3];}

	//const TELEM*	cTab() const	{ return mtab;}
	const TELEM*	cTab() const	{ return mtab.data();}

	void	Write( TSize iprec=5, std::ostream& f=vcout) const;

protected:
	// 
	std::array< TELEM, (MAX_SIZE>0?MAX_SIZE:1) >	mtab;
	//TELEM mtab[MAX_SIZE>0?MAX_SIZE:1];

};
//////////////////////////////////////////////////////////////////////







template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE>& GVector<TELEM,MAX_SIZE>::operator =( const GVector<TELEM,MAX_SIZE> &vec)
{
	for ( TSize i=0; i<MAX_SIZE; ++i)
		mtab[i] = vec.mtab[i];
	return *this;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE>& GVector<TELEM,MAX_SIZE>::operator+=( const GVector<TELEM,MAX_SIZE>& vec)
{
	for ( TSize i=0; i<MAX_SIZE; ++i)
		mtab[i] += vec.mtab[i];
	return *this;
}


template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE>& GVector<TELEM,MAX_SIZE>::operator-=( const GVector<TELEM,MAX_SIZE>& vec)
{
	for ( TSize i=0; i<MAX_SIZE; ++i)
		mtab[i] -= vec.mtab[i];
	return *this;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE>& GVector<TELEM,MAX_SIZE>::operator*=( const TELEM& doub)
{
	for ( TSize i=0; i<MAX_SIZE; ++i)
		mtab[i] *= doub;
	return *this;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE>& GVector<TELEM,MAX_SIZE>::operator/=( const TELEM& doub)
{
	for ( TSize i=0; i<MAX_SIZE; ++i)
		mtab[i] /= doub;
	return *this;
}


template <class TELEM, TSize MAX_SIZE>
inline void GVector<TELEM,MAX_SIZE>::Write( TSize iprec, std::ostream& f) const
{
	f << "[ ";
	for( TSize i=0; i<MAX_SIZE; i++)
		f << /*setw(12) <<*/ setprecision(iprec) << cX(i) << " ";
	f << "]" << endl;
}

template <class TELEM, TSize MAX_SIZE>
inline std::ostream& operator << ( std::ostream& f, const GVector<TELEM,MAX_SIZE>& vct)
{
	f << "[ ";
	for( TSize i=0; i<MAX_SIZE; i++)
		f << /*setw(12) <<*/ vct.cX(i) << " ";
	f << "]";
	return f;
}



//////////////////////////////////////////////////////////////////////

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> operator-( const GVector<TELEM,MAX_SIZE>& vec)
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = - vec.mtab[i];
	return v;
}


template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> operator-( const GVector<TELEM,MAX_SIZE>& vec1, const GVector<TELEM,MAX_SIZE>& vec2)
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = vec1.mtab[i] - vec2.mtab[i];
	return v;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> operator+( const GVector<TELEM,MAX_SIZE>& vec1, const GVector<TELEM,MAX_SIZE>& vec2)
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = vec1.mtab[i] + vec2.mtab[i];
	return v;
}

template <class TELEM, TSize MAX_SIZE>
inline TELEM operator*( const GVector<TELEM,MAX_SIZE>& vec1, const GVector<TELEM,MAX_SIZE>& vec2)
{
	TELEM	e(0);
	for ( TSize i=0; i<MAX_SIZE; ++i)
		e += vec1.mtab[i] * vec2.mtab[i];
	return e;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> operator*( const TELEM& e, const GVector<TELEM,MAX_SIZE>& vec)
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = e * vec.mtab[i];
	return v;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> operator*( const GVector<TELEM,MAX_SIZE>& vec, const TELEM& e)
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = e * vec.mtab[i];
	return v;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> operator/( const GVector<TELEM,MAX_SIZE>& vec, const TELEM& e)
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = vec.mtab[i] / e;
	return v;
}


template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> Minimum(  const GVector<TELEM,MAX_SIZE>& vec1,  const GVector<TELEM,MAX_SIZE>& vec2 )
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = ( vec1.mtab[i] < vec2.mtab[i] )  ?  vec1.mtab[i]  : vec2.mtab[i];
	return v;
}

template <class TELEM, TSize MAX_SIZE>
inline GVector<TELEM,MAX_SIZE> Maximum(  const GVector<TELEM,MAX_SIZE>& vec1,  const GVector<TELEM,MAX_SIZE>& vec2 )
{
	GVector<TELEM,MAX_SIZE>	v;
	for ( TSize i=0; i<MAX_SIZE; ++i)
		v.mtab[i] = ( vec1.mtab[i] > vec2.mtab[i] )  ?  vec1.mtab[i]  : vec2.mtab[i];
	return v;
}



template <class TELEM>
inline TELEM operator%( const GVector<TELEM,2>& vec1, const GVector<TELEM,2>& vec2)
{
	TELEM z = vec1.cX(0)*vec2.cX(1) - vec1.cX(1)*vec2.cX(0);
	return z;
}


template <class TELEM>
inline GVector<TELEM,3> operator%( const GVector<TELEM,3>& vec1, const GVector<TELEM,3>& vec2)
{
	TELEM x = vec1.cX(1)*vec2.cX(2) - vec1.cX(2)*vec2.cX(1);
	TELEM y = vec1.cX(2)*vec2.cX(0) - vec1.cX(0)*vec2.cX(2);
	TELEM z = vec1.cX(0)*vec2.cX(1) - vec1.cX(1)*vec2.cX(0);
	return GVector<TELEM,3>({x,y,z});
}



template <class TELEM, TSize MAX_SIZE>
inline bool operator == ( const GVector<TELEM,MAX_SIZE>& v1, const GVector<TELEM,MAX_SIZE>& v2 )
{
	for ( TSize i=0; i<MAX_SIZE; ++i)
		if ( v1.cX(i) != v2.cX(i) )
			return false;

	return true;
}


template <class TELEM, TSize MAX_SIZE>
inline bool operator < ( const GVector<TELEM,MAX_SIZE>& v1, const GVector<TELEM,MAX_SIZE>& v2 )
{
	if ( v1.cX() < v2.cX() )
		return true;

	if ( v1.cX() > v2.cX() )
		return false;

	GVector<TELEM,MAX_SIZE-1> w1, w2;

	for ( TSize i=1; i<MAX_SIZE; ++i)
	{
		w1.rX( i-1 ) = v1.cX(i);
		w2.rX( i-1 ) = v2.cX(i);
	}

	return w1 < w2;
}


template <class TELEM>
inline bool operator < ( const GVector<TELEM,1>& v1, const GVector<TELEM,1>& v2 )
{
	return v1.cX() < v2.cX();
}


template <TSize N>
using GFloatVector = GVector<Float64,N>;

using Vect1D = GVector<Float64,DIM_1D>;
using Vect2D = GVector<Float64,DIM_2D>;
using Vect3D = GVector<Float64,DIM_3D>;
using Vect4D = GVector<Float64,DIM_4D>;


} // end of namespace CFGeomTools
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

namespace CFGeom = CFGeomTools;



#endif // __CF_GVECTOR_H__
