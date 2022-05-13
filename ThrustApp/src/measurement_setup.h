#ifndef __MEASUREMENT_SETUP_H__
#define __MEASUREMENT_SETUP_H__

#include <QObject>


enum class ERpmSource
{
	None		= 0,
	IrSensor	= 1,
	EscTelem	= 2
};

enum class ECurrZeroSource
{
	None	= 0,
	Auto	= 1,
	Sensor	= 2,
	Manual	= 3
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class MeasurementSetup
////////////////////////////////////////////////////////////////////////////////////////////////////
class MeasurementSetup
{
	friend QDataStream &operator<<( QDataStream &, const MeasurementSetup &);
	friend QDataStream &operator>>( QDataStream &, MeasurementSetup &);

public:
	MeasurementSetup();

	void	setDefaults();

	const QString&		cMotorName() const		{ return mMotorName; }
	int					cMotorKV() const		{ return mMotorKV; }
	int					cMotorPoles() const		{ return mMotorPoles; }
	const QString&		cPropName() const		{ return mPropName; }
	int					cPropBlades() const		{ return mPropBlades; }
	const QString&		cComment() const		{ return mComment; }

	ERpmSource			cRpmSource() const		{ return mRpmSource; }
	ECurrZeroSource		cCurZeroSource() const	{ return mCurZeroSource; }

	bool				cEscTelemEnabled() const	{ return mbEnableEscTelem; }
//	bool				cCurZeroEnabled() const		{ return mbEnableCurrZero; }


	QString&			rMotorName()			{ return mMotorName; }
	int&				rMotorKV()				{ return mMotorKV; }
	int	&				rMotorPoles()			{ return mMotorPoles; }
	QString&			rPropName()				{ return mPropName; }
	int&				rPropBlades()			{ return mPropBlades; }
	QString&			rComment()				{ return mComment; }

	ERpmSource&			rRpmSource()			{ return mRpmSource; }
	ECurrZeroSource&	rCurZeroSource()		{ return mCurZeroSource; }

	bool&				rEscTelemEnabled()		{ return mbEnableEscTelem; }
	//bool&				rCurZeroEnabled()		{ return mbEnableCurrZero; }

private:
	// motor info
	QString		mMotorName;
	int			mMotorKV;
	int			mMotorPoles;
	QString		mPropName;
	int			mPropBlades;
	QString		mComment;

	ERpmSource		mRpmSource;
	ECurrZeroSource	mCurZeroSource;

	bool		mbEnableEscTelem;
	//bool		mbEnableCurrZero;
};

QDataStream &operator<<( QDataStream &, const MeasurementSetup &);
QDataStream &operator>>( QDataStream &, MeasurementSetup &);


#endif // __MEASUREMENT_SETUP_H__
