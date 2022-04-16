#ifndef __RESULT_H__
#define __RESULT_H__

#include <array>
#include <vector>

#include <QDataStream>
#include <QTextStream>

#include "statistic_value.h"


class MeasurementData;

enum EResult
{
	RES_THRUST_ID = 0,
	RES_TORQUE_ID,
	RES_RPM_ID,
	RES_BATT_VOLT_ID,
	RES_BATT_CURR_ID,
	RES_CH_COUNT,					// count of measured data

	RES_POWER_IN_ID = RES_CH_COUNT,
	//RES_POWER_OUT_ID,
	//RES_EFFICIENCY_ID,
	RES_THRUST_EFF_ID,

	RES_COUNT						// count of all data
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ResultRow
////////////////////////////////////////////////////////////////////////////////////////////////////
class ResultRow
{
public:
	using TData = std::array<StatisticValue, RES_COUNT>;

	double&			rLevel()			{ return mLevel; }
	double			cLevel() const		{ return mLevel; }

	TData& 			rData()				{ return mtab; }
	const TData&	cData() const		{ return mtab; }

	void	UpdateDepValues();

private:
	double	mLevel;
	TData	mtab;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class Result
////////////////////////////////////////////////////////////////////////////////////////////////////
class Result
{
public:
	using TRange = std::pair<int, int>;
	using TLevelDef = std::pair<int, TRange>;

public:
	void	Reset()
	{
		mtabMarks.clear();
		mtabRes.clear();
	}

	void	WriteData( QTextStream& out, const MeasurementData* pdata) const;
	void	WriteProcessedData( QTextStream& out) const;
	void	WriteDataTable( std::ofstream& fout, const std::string& sep, const std::string& end) const; 

	const std::vector< TLevelDef >&	cTabMarks() const		{ return mtabMarks; }
	const std::vector< ResultRow >&	cTabResult() const		{ return mtabRes; }

	std::vector< TLevelDef >&	rTabMarks()		{ return mtabMarks; }
	std::vector< ResultRow >&	rTabResult()	{ return mtabRes; }

	const QString&	cText() const		{ return mText; }
	QString&		rText() 			{ return mText; }

private:
	QString mText;

	std::vector< TLevelDef >	mtabMarks;	// level, ibeg, iend
	std::vector< ResultRow >	mtabRes;
};

QDataStream &operator<<( QDataStream &, const Result &);
QDataStream &operator>>( QDataStream &, Result &);


////////////////////////////////////////////////////////////////////////////////////////////////////
// class Result
////////////////////////////////////////////////////////////////////////////////////////////////////
class ResultSet
{
	friend QDataStream &operator<<( QDataStream &, const ResultSet &);
	friend QDataStream &operator>>( QDataStream &, ResultSet &);

public:
	void	Reset()							{ mtabRes.clear(); }
	void	WriteData( QTextStream& out, const MeasurementData* pdata) const;

	void	AddItem( const Result& res)		{ mtabRes.push_back(res); }
	size_t	Size() const					{ return mtabRes.size(); }

	Result&			rItem( size_t i)		{ return mtabRes[i]; }
	const Result&	cItem( size_t i) const	{ return mtabRes[i]; }

private:
	std::vector<Result>		mtabRes;
};

QDataStream &operator<<( QDataStream &, const ResultSet &);
QDataStream &operator>>( QDataStream &, ResultSet &);



#endif // __RESULT_H__
