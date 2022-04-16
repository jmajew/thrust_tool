#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include <QDebug>
#include <QtGlobal>
#include <QObject>

#include "result.h"

class MeasurementData;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class Analyzer
////////////////////////////////////////////////////////////////////////////////////////////////////
class Analyzer : public QObject
{
    Q_OBJECT

	using TRange = Result::TRange;
	using TLevelDef = Result::TLevelDef;

public:
	explicit Analyzer();

	void	DoAnalyze( ResultSet& res, const TRange& range, const MeasurementData* pdata);

	void	SetMargins( int l, int r)	{ mLeftMarg =l; mRightMarg = r; }

public slots:

private:
	void	FindMarks( std::vector< TLevelDef >& tab);
	void	AnalyzeItem( Result& res);

	StatisticValue	ComputeChannel( int ch, const TLevelDef& def);

private:
	TRange	mRange;
	int		mLeftMarg;
	int		mRightMarg;

	const MeasurementData*	mpData;
};


#endif // __ANALYZER_H__
