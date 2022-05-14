#include <QFile>
#include <QDataStream>

#include "measurement_setup.h"

MeasurementSetup::MeasurementSetup()
{
	setDefaults();
}


void MeasurementSetup::setDefaults()
{
	mMotorName = "Generic Motor";
	mMotorKV = 0;
	mMotorPoles = 14;
	mPropName = "Generic Propeller";
	mPropBlades = 2;
	mComment = "";

	mRpmSource = ERpmSource::IrSensor;
	mCurZeroSource = ECurrZeroSource::Auto;

	mbEnableEscTelem = false;
	//mbEnableCurrZero = false;
}


QDataStream& operator<<( QDataStream& out, const MeasurementSetup& setup)
{
	out << setup.mMotorName;
	out << setup.mMotorKV;
	out << setup.mMotorPoles;
	out << setup.mPropName;
	out << setup.mPropBlades;
	out << setup.mComment;

	out << static_cast<quint32>( setup.mRpmSource );
	out << static_cast<quint32>( setup.mCurZeroSource );

	out << setup.mbEnableEscTelem;
	//out << setup.mbEnableEscTelem;

	return out;
}

QDataStream& operator>>( QDataStream& in, MeasurementSetup& setup)
{
	in >> setup.mMotorName;
	in >> setup.mMotorKV;
	in >> setup.mMotorPoles;
	in >> setup.mPropName;
	in >> setup.mPropBlades;
	in >> setup.mComment;
	
	int isrc;
	in >> isrc;
	if ( isrc == 1)
		setup.mRpmSource = ERpmSource::IrSensor;
	else if ( isrc == 2 )
		setup.mRpmSource = ERpmSource::EscTelem;
	else
		setup.mRpmSource = ERpmSource::None;

	//in >> isrc;
	//if ( isrc == 1)
	//	setup.mCurZeroSource = ECurrZeroSource::Auto;
	//else if ( isrc == 2 )
	//	setup.mCurZeroSource = ECurrZeroSource::Sensor;
	//else if ( isrc == 3 )
	//	setup.mCurZeroSource = ECurrZeroSource::Manual;
	//else
	//	setup.mCurZeroSource = ECurrZeroSource::None;


	in >> setup.mbEnableEscTelem;
	//in >> setup.mbEnableCurrZero;
	in >> isrc; // HACK :: for compat. purposes

	return in;
}
