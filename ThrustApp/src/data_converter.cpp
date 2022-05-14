#include "data_converter.h"
#include "appsetup.h"


int batteryNCellDetect( double vbat)
{
    int batteryCellCount = (vbat / 4.25) + 1;
    return std::min(batteryCellCount, 6);
}




double DataConverter::convertVBat(uint16_t ibat, uint16_t iref) const
{
	const double adc_coeff = mVref / iref;
	double v = ibat * adc_coeff * mVbatDivider * mVbatCorr;	// [V]

	int ncell = batteryNCellDetect( v);

	return v/ncell;
}

void DataConverter::init( const MeasurementSetup& mesetup, const ToolSetup& setup)
{
	meCurZero = mesetup.cCurZeroSource();
	meRpmSrc = mesetup.cRpmSource();
	mnMagnetPoles = mesetup.cMotorPoles();
	mnPropBlades = mesetup.cPropBlades();

	mVref = (double)setup.cConfig().groupConvert.Vref / 1000.0;
	mVbatDivider = (double)setup.cConfig().groupConvert.VbatDivider / 1000.0;
//	mVbatCorr = 10000.0 / (double)setup.cConfig().groupConvert.VbatCorr;
	mVbatCorr = (double)setup.cConfig().groupConvert.VbatCorr / 10000.0;
	mLengthTq = (double)setup.cConfig().groupConvert.LeverArmLength / 100.0;	// <-- mm
	mCoeffCurr = (double)setup.cConfig().groupConvert.VIcoeff / 100.0;
	mRawCurr0 = setup.cConfig().groupConvert.RawIzero;

	//(*ts_cal2 - *ts_cal1) / ( 110 - 30 );
	mTempSlope = (setup.cConfig().groupConvert.ts_cal2 - setup.cConfig().groupConvert.ts_cal1) / (110.-30.);
	mTempOff = setup.cConfig().groupConvert.ts_cal1;


	// TODO :: id of stgauge channel should be read from setup
	mtabCoeffStG[0] = (double)setup.cConfig().groupConvert.tabStGScale[0] / 1000.;
	mtabCoeffStG[1] = (double)setup.cConfig().groupConvert.tabStGScale[1] / 1000.;

	mtabOffsetStG[0] = (double)setup.cConfig().groupConvert.tabStGZero[0];
	mtabOffsetStG[1] = (double)setup.cConfig().groupConvert.tabStGZero[1];
}


void DataConverter::convert( RawData& data ) const
{
	data[CH_EXT_MOTOR_OUT_ID] = (data[CH_MOTOR_OUT_ID] - 1000.) / 10.;

	data[CH_EXT_THRUST_ID] = ( data[CH_STG_THRUST_ID] - mtabOffsetStG[0] ) / mtabCoeffStG[0];	// thrust: force [G]
	data[CH_EXT_TORQUE_ID] = ( data[CH_STG_TORQUE_ID] - mtabOffsetStG[1] ) / mtabCoeffStG[1];	// torque: force [G]

	data[CH_EXT_TORQUE_ID] *= mLengthTq / 10.;	// torque [G * cm]

	if ( meRpmSrc == ERpmSource::IrSensor )
	{
		data[CH_EXT_RPM_ID]		= data[CH_RPM_ID] / mnPropBlades;		// sensor rpm
	}
	else 
	if ( meRpmSrc == ERpmSource::EscTelem )
	{
		data[CH_EXT_RPM_ID]		= data[CH_ESCT_RPM_ID] * 200. / mnMagnetPoles;		// Esc Telem rpm [1/min]
	}
	else
	{	// error	
	}

	// mpu temp
	//const float ts_slope = (*ts_cal2 - *ts_cal1) / ( 110 - 30 );
	//return (uint16_t)( ( ((float)val - (float)*ts_cal1) / ts_slope + 30.0 ) * 100.0 );
	data[CH_EXT_MPUTEMP_ID] = (data[CH_ADC_MPUTEMP_ID] - mTempOff) / mTempSlope + 30.0;		// [deg C]

	// battery voltage
	const double adc_coeff = mVref / data[CH_ADC_VREF_ID];
	data[CH_EXT_BATT_VOLT_ID] = data[CH_ADC_VBATT_ID] * adc_coeff * mVbatDivider * mVbatCorr;	// [V]

	// current
	double m_curr0 = 0;
	if ( meCurZero == ECurrZeroSource::Auto )
		m_curr0 = 2048;
	else if ( meCurZero == ECurrZeroSource::Sensor )
		m_curr0 = data[CH_ADC_IBATTZERO_ID];
	else if ( meCurZero == ECurrZeroSource::Manual )
		m_curr0 = mRawCurr0;

	data[CH_EXT_BATT_CURR_ID] = mCoeffCurr * (data[CH_ADC_IBATT_ID] - m_curr0) / 4096.0;	// [A]

	// calc others
	data[CH_EXT_POWER_IN_ID] = data[CH_EXT_BATT_VOLT_ID] * data[CH_EXT_BATT_CURR_ID];

	data[CH_EXT_POWER_OUT_ID] = data[CH_EXT_TORQUE_ID] * data[CH_EXT_RPM_ID] / 60.0;
	data[CH_EXT_POWER_OUT_ID] /= 10000; // convert to N*m/s

	data[CH_EXT_EFFICIENCY_ID] = 0.;

	if ( data[CH_EXT_POWER_IN_ID] > 0.1 )
	{
		data[CH_EXT_THRUST_EFF_ID] = data[CH_EXT_THRUST_ID] / data[CH_EXT_POWER_IN_ID];
		data[CH_EXT_EFFICIENCY_ID] = data[CH_EXT_POWER_OUT_ID] / data[CH_EXT_POWER_IN_ID];
	}
	else 
	{
		data[CH_EXT_THRUST_EFF_ID] = 0.;
		data[CH_EXT_EFFICIENCY_ID] = 0.;
	}

	constexpr size_t ibegStG	= MOTOR_CH_COUNT;
	constexpr size_t ibegAdc	= ibegStG + STGAUGE_CH_COUNT;
	constexpr size_t ibegEsct	= ibegAdc + RPM_CH_COUNT + STGAUGE_CH_COUNT;

	////ESC Telemetry
	//tabOut[CH_ESCT_VBATT_ID]	= tabIn[CH_ESCT_VBATT_ID] * 0.01;
	//tabOut[CH_ESCT_IBATT_ID]	= tabIn[CH_ESCT_IBATT_ID] * 0.01;
	//tabOut[CH_ESCT_RPM_ID]	= tabIn[CH_ESCT_RPM_ID] * 100. / (mnMagnetPoles/2);
	//tabOut[CH_ESCT_TEMP_ID]	= tabIn[CH_ESCT_TEMP_ID];
}



QDataStream& operator<<( QDataStream& out, const DataConverter& datc)
{
	out << datc.mVref;
	out << datc.mCoeffADC;
	out << datc.mVbatDivider;
	out << datc.mVbatCorr;
	out << datc.mCoeffCurr;
	out << datc.mVoltCurr0;

	for ( int i=0; i<STGAUGE_CH_COUNT; ++i )
	{
		out << datc.mtabCoeffStG[STGAUGE_CH_COUNT];
		out << datc.mtabOffsetStG[STGAUGE_CH_COUNT];
	}
	out << datc.mLengthTq;

	out << datc.meCurZero;
	out << datc.meRpmSrc;
	out << datc.mnMagnetPoles;
	out << datc.mnPropBlades;

	return out;
}

QDataStream& operator>>( QDataStream& in, DataConverter& datc)
{
	in >> datc.mVref;
	in >> datc.mCoeffADC;
	in >> datc.mVbatDivider;
	in >> datc.mVbatCorr;
	in >> datc.mCoeffCurr;
	in >> datc.mVoltCurr0;

	for ( int i=0; i<STGAUGE_CH_COUNT; ++i )
	{
		in >> datc.mtabCoeffStG[STGAUGE_CH_COUNT];
		in >> datc.mtabOffsetStG[STGAUGE_CH_COUNT];
	}
	in >> datc.mLengthTq;

	//// HACK
	//{
	//	bool b;
	//	in >> b;
	//	datc.meCurZero = ECurrZeroSource::Auto;
	//}

	in >> datc.meCurZero;
	in >> datc.meRpmSrc;

	in >> datc.mnMagnetPoles;
	in >> datc.mnPropBlades;

	return in;
}
