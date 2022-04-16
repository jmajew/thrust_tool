#ifndef __STATISTIC_VALUE_H__
#define __STATISTIC_VALUE_H__

#include <math.h>

#include <QDataStream>


////////////////////////////////////////////////////////////////////////////////////////////////////
// class StatisticValue
////////////////////////////////////////////////////////////////////////////////////////////////////
class StatisticValue
{
	friend QDataStream& operator<<(QDataStream& out, const StatisticValue& res);
	friend QDataStream& operator>>(QDataStream& in, StatisticValue& res);

public:
	StatisticValue( size_t m=0, double sum=0., double sumsq=0. ) : n(m), sx(sum), sx2(sumsq)	{}
	StatisticValue( double val ) : n(1), sx(val), sx2( val * val )				{}

	StatisticValue( const StatisticValue& ) = default;

	void Reset()
	{
		this->n = 0;
		this->sx = 0;
		this->sx2 = 0;
	}

	void Accumulate( double val )
	{
		this->n += 1;
		this->sx += val;
		this->sx2 += val*val;
	}

	void Accumulate( const StatisticValue& val )
	{
		this->n += val.n;
		this->sx += val.sx;
		this->sx2 += val.sx2;
	}

	size_t	Count() const					{ return n; }

	double	Mean() const					{ return sx / static_cast<double>(n); }

	double	Variance() const
	{
		double mean = this->Mean();
		return ( sx2 / static_cast<double>(n) - mean*mean );
	}

	double	Deviation() const				{ return sqrt( this->Variance() ); }

public:

	//double	cSumX() const					{ return sx; }
	//double	cSumX2() const					{ return sx2; }

private:
	size_t	n;		// sum_i 1
	double	sx;		// sum_i x_i
	double	sx2;	// sum_i x_i^2
};


inline QDataStream& operator<<(QDataStream& out, const StatisticValue& val)
{	
	out << val.n;
	out << val.sx;
	out << val.sx2;

	return out;
}

inline QDataStream& operator>>(QDataStream& in, StatisticValue& val)
{
	in >> val.n;
	in >> val.sx;
	in >> val.sx2;
	return in;
}



#endif // __STATISTIC_VALUE_H__
