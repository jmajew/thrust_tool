#ifndef __DATA_CONVERTER_H__
#define __DATA_CONVERTER_H__

#include <QObject>
#include <QDataStream>

#include "measurement_setup.h"
#include "measure_data.h"

class ToolSetup;
class MeasurementSetup;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class DataConverter
////////////////////////////////////////////////////////////////////////////////////////////////////
class DataConverter : public QObject
{
    Q_OBJECT

	friend QDataStream& operator<<( QDataStream& out, const DataConverter& datc);
	friend QDataStream& operator>>( QDataStream& in, DataConverter& datc);

public:
	DataConverter( QObject *parent) : QObject(parent)	{}

	void	init( const MeasurementSetup& mesetup, const ToolSetup& setup);
	//void	convert( std::vector<double>& tabOut, const std::vector<double>& tabIn) const;
	void	convert( RawData& data) const;

	double	convertVBat( uint16_t ibat, uint16_t iref) const;

private:
	double	mVref;
	double	mCoeffADC;
	double	mVbatDivider;
	double	mVbatCorr;
	double	mCoeffCurr;
	double	mVoltCurr0;		// if negative use measured CH_ADC_IBATTZERO_ID

	double	mtabCoeffStG[STGAUGE_CH_COUNT];
	double	mtabOffsetStG[STGAUGE_CH_COUNT];
	double	mLengthTq;

	bool		mbCurZero;
	ERpmSource	mRpmSrc;
	qint32		mnMagnetPoles;
	qint32		mnPropBlades;
	float		mTempSlope;
	float		mTempOff;
};

QDataStream& operator<<( QDataStream& out, const DataConverter& datc);
QDataStream& operator>>( QDataStream& in, DataConverter& datc);


#endif // __DATA_CONVERTER_H__

