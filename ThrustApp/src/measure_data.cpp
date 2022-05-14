#include <QDebug>
#include <QFile>
#include <QDataStream>

#include "document.h"
#include "measure_data.h"


const DefChannel tabDefChannel[] =
{
	// raw
	{ CH_MOTOR_OUT_ID,		"Motor Output",				0,			2000},

	{ CH_STG_THRUST_ID,		"Strain Gauge Thrust",		-5000000,	5000000},
	{ CH_STG_TORQUE_ID,		"Strain Gauge Torque",		-5000000,	5000000},

	{ CH_ADC_VREF_ID,		"Vref",						0,			4096},
	{ CH_ADC_VBATT_ID,		"VoltBatt",					0,			4096},
	{ CH_ADC_IBATT_ID,		"CurrBatt",					0,			4096},
	{ CH_ADC_IBATTZERO_ID,	"CurrBatt Zero",			0,			4096},
	{ CH_ADC_MPUTEMP_ID,	"MPU Temperature",			0,			4096},

	{ CH_RPM_ID,			"RPM",						0,			50000},

	{ CH_ESCT_VBATT_ID,		"ESCT Volt",				0,			300},
	{ CH_ESCT_IBATT_ID,		"ESCT Curr",				0,			100},
	{ CH_ESCT_RPM_ID,		"ESCT RPM",					0,			40},
	{ CH_ESCT_TEMP_ID,		"ESCT Temp",				0,			10},

	// cooked
	{ CH_EXT_MOTOR_OUT_ID,	"Motor Level [%]",			0,			100},

	{ CH_EXT_THRUST_ID,		"Thrust [G]",				0,			1000},
	{ CH_EXT_TORQUE_ID,		"Torque [G cm]",			0,			3500},
	{ CH_EXT_RPM_ID,		"RPM [1/min]",				0,			50000},

	{ CH_EXT_BATT_VOLT_ID,	"VoltBatt [V]",				0,			30},
	{ CH_EXT_BATT_CURR_ID,	"CurrBatt [A]",				0,			15},
	{ CH_EXT_MPUTEMP_ID,	"MPU Temp",					0,			100},

	{ CH_EXT_POWER_IN_ID,	"Power In [W]",				0,			500},
	{ CH_EXT_POWER_OUT_ID,	"Power Out [W]",			0,			500},
	{ CH_EXT_EFFICIENCY_ID,	"Efficiency [%]",			0,			100},
	{ CH_EXT_THRUST_EFF_ID,	"Thrust Efficiency [G/W]",	0,			20},
};

const uint16_t sizeDefChannel = ARRAYLEN(tabDefChannel);




//void RawDataInfo::UpdateValues( RawData& data )
//{
//	for (size_t ich = 0; ich < DATA_SIZE; ++ich)
//		mtabInfo[ich].Update( data[ich] );
//}

////////////////////////////////////////////////////////////////////////////////////////////////////
// class MeasurementData
////////////////////////////////////////////////////////////////////////////////////////////////////

MeasurementData::MeasurementData( QObject *parent ) 
	: QAbstractTableModel(parent), mbIsReady(false), mSizeCh(0), mSizeData(0)
{
	this->initChannels();
	this->setDefaults();
}


bool MeasurementData::checkSizes() const
{
	if ( mSizeCh != mtabChannel.size() )
		return false;

	if ( ! mbIsReady || mtabChannel.size() == 0 ) // TODO :: ensure: if mbIsReady => nch != 0
		return true;

	for ( size_t i = 0; i < mtabChannel.size(); ++i )
	{
		if ( mSizeData != mtabChannel[i].size() )
			return false;
	}

	return true;
}

void MeasurementData::initChannels()
{
//	size_t rawCount = MOTOR_CH_COUNT + STGAUGE_CH_COUNT + ADC_CH_COUNT + RPM_CH_COUNT + ESCTELEM_CH_COUNT;

	mSizeCh = CHANNEL_COUNT;
	assert( sizeDefChannel == mSizeCh );
	
	mtabChannel.resize( mSizeCh );

	for ( size_t i = 0; i < sizeDefChannel; ++i)
	{
		mtabChannel[ tabDefChannel[i].id ].setId( tabDefChannel[i].id );
		mtabChannel[ tabDefChannel[i].id ].setName( tabDefChannel[i].name );
		mtabChannel[ tabDefChannel[i].id ].setRange( tabDefChannel[i].valMin, tabDefChannel[i].valMax );

		if ( tabDefChannel[i].id < CH_EXT_MOTOR_OUT_ID )
			mtabChannel[ tabDefChannel[i].id ].setType( EDataType::Raw );
		else
			mtabChannel[ tabDefChannel[i].id ].setType( EDataType::Cooked );
	}


	// ...
	mbIsReady = true;
}

void MeasurementData::setDefaults()
{
	for ( size_t i = 0; i < sizeDefChannel; ++i)
	{
		mtabChannel[ tabDefChannel[i].id ].setName( tabDefChannel[i].name );
		mtabChannel[ tabDefChannel[i].id ].setRange( tabDefChannel[i].valMin, tabDefChannel[i].valMax );
	}
}


QVariant MeasurementData::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) 
	{
		return mtabChannel[ section ].getName();
		//return getChName( section );
    }
	else if (role == Qt::DisplayRole && orientation == Qt::Vertical)
	{
		return QString::number(section);
	}

    return QVariant();
}


QDataStream& operator<<(QDataStream& out, const DataChannel& ch)
{
	out << ch.mId;
	out << ch.mParentId;
	out << ch.mName;
	out << ch.mRange.cMin() << ch.mRange.cMax();

	out << ch.mtabData.size();
	for ( size_t i=0; i<ch.mtabData.size(); ++i)
		out << ch.mtabData[i];

	return out;
}

QDataStream& operator>>(QDataStream& in, DataChannel& ch)
{
	in >> ch.mId;
	in >> ch.mParentId;
	in >> ch.mName;

	double dmin, dmax;
	in>> dmin >> dmax;
	ch.setRange( dmin, dmax);

	size_t n;
	in >> n;

	ch.mtabData.resize(n);
	for ( size_t i=0; i<ch.mtabData.size(); ++i)
		in >> ch.mtabData[i];

	return in;
}


QDataStream& operator<<( QDataStream& out, const MeasurementData& datm)
{
	assert( datm.mSizeCh == datm.mtabChannel.size() );

	out << datm.mSizeCh;
	out << datm.mSizeData;
	//out << datm.mText;

	for ( size_t i=0; i<datm.mtabChannel.size(); ++i)
	{
		out << datm.mtabChannel[i];
	}

	return out;
}

QDataStream& operator>>( QDataStream& in, MeasurementData& datm)
{
	in >> datm.mSizeCh;
	in >> datm.mSizeData;
	//in >> datm.mText;

	datm.mtabChannel.resize(datm.mSizeCh);

	for ( size_t i=0; i<datm.mtabChannel.size(); ++i)
	{
		in >> datm.mtabChannel[i];
	}

	if (!datm.checkSizes())
	{
		qDebug() << "MeasurementData :: operator >> :: corrupted sizes";
	}

	return in;
}
