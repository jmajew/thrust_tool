#ifndef __MEASURE_DATA_H__
#define __MEASURE_DATA_H__


#include <array>
#include <vector>

#include <QDebug>
#include <QtGlobal>
#include <QObject>
#include <QAbstractTableModel>

#include "sysdecl.h"
#include "thsp_packet.hpp"
#include "stat_value.hpp"
#include "config.hpp"


class Document;



// global channel ids
enum EChannelId
{
// raw
	CH_MOTOR_OUT_ID			= 0,

	CH_STG_THRUST_ID,
	CH_STG_TORQUE_ID,

	CH_ADC_VREF_ID,
	CH_ADC_VBATT_ID,
	CH_ADC_IBATTZERO_ID,
	CH_ADC_IBATT_ID,
	CH_ADC_MPUTEMP_ID,

	CH_RPM_ID,

	CH_ESCT_VBATT_ID,
	CH_ESCT_IBATT_ID,
	CH_ESCT_RPM_ID,
	CH_ESCT_TEMP_ID,

	CHANNEL_RAW_COUNT,

// cooked
	CH_EXT_MOTOR_OUT_ID		= CHANNEL_RAW_COUNT,

	CH_EXT_THRUST_ID,
	CH_EXT_TORQUE_ID,
	CH_EXT_RPM_ID,

	CH_EXT_BATT_VOLT_ID,
	CH_EXT_BATT_CURR_ID,
	CH_EXT_MPUTEMP_ID,

	CH_EXT_POWER_IN_ID,
	CH_EXT_POWER_OUT_ID,
	CH_EXT_EFFICIENCY_ID,
	CH_EXT_THRUST_EFF_ID,

	CHANNEL_COUNT
};


struct DefChannel
{
	const size_t	id;
	const char*		name;
	double			valMin;
	double			valMax;
};


extern const DefChannel	tabDefChannel[];
extern const uint16_t	sizeDefChannel;


enum class EDataType
{
	Unknown	= -1,
	Raw		= 0,
	Cooked	= 1
};



////////////////////////////////////////////////////////////////////////////////////////////////////
// class RawData
////////////////////////////////////////////////////////////////////////////////////////////////////
class RawData : public std::array< double, CHANNEL_COUNT>
{
public:
	RawData( size_t id) : mId(id)	{}

	size_t	getId() const			{ return mId; }

private:
	size_t	mId;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ChannelInfo
////////////////////////////////////////////////////////////////////////////////////////////////////
class ChannelRange
{
public:
	ChannelRange() : mvalMin(0), mvalMax(0)	{}

	void	setRange( double vmin, double vmax)
	{
		mvalMin = vmin;
		mvalMax = vmax;
	}

	void	update( double val)
	{
		if ( val < mvalMin )	mvalMin = val;
		if ( val > mvalMax )	mvalMax = val;
	}

	double	cMin() const	{ return mvalMin; }
	double	cMax() const	{ return mvalMax; }

private:
    double				mvalMin;
    double				mvalMax;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ChannelStyle
////////////////////////////////////////////////////////////////////////////////////////////////////
class ChannelStyle
{
public:
private:
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class DataChannel
////////////////////////////////////////////////////////////////////////////////////////////////////
class DataChannel //: public QObject
{
	//Q_OBJECT

	friend QDataStream& operator<<( QDataStream& out, const DataChannel& ch);
	friend QDataStream& operator>>( QDataStream& in, DataChannel& ch);

public:
	//DataChannel( QObject *parent = nullptr ) : QObject(parent)		{}

	size_t	size() const	{ return mtabData.size(); }


	size_t					getId() const			{ return mId; }
	QString					getName() const			{ return mName; }
	const ChannelRange&		getRange() const		{ return mRange; }
	EDataType				getType() const			{ return mType; }

	const double&	operator[]( size_t i ) const	{ return mtabData[i]; }


	void	setId( size_t id )						{ mId = id; }
	void	setRange( double vmin, double vmax )	{ mRange.setRange( vmin, vmax); }
	void	setName( const char* txt )				{ mName = txt; }
	void	setType( EDataType type )				{ mType = type; }


	void	update( double value)
	{
		mtabData.push_back( value );
		mRange.update( value);
	}

private:
	size_t			mId;
	size_t			mParentId;
	QString			mName;

	EDataType			mType;
	ChannelRange		mRange;
//	ChannelStyle		mStyle;

	// TODO :: change vector to array
	std::vector<double>	mtabData;
};

QDataStream& operator<<( QDataStream& out, const DataChannel& ch);
QDataStream& operator>>( QDataStream& in, DataChannel& ch);


////////////////////////////////////////////////////////////////////////////////////////////////////
// class MeasurementData
////////////////////////////////////////////////////////////////////////////////////////////////////
class MeasurementData : public QAbstractTableModel
{
	Q_OBJECT

	friend QDataStream& operator<<( QDataStream &out, const MeasurementData &datm);
	friend QDataStream& operator>>( QDataStream &in, MeasurementData &datm);

public:

	explicit MeasurementData( QObject *parent = nullptr);

//------------------------------------------------------------------------------------------------//
// QAbstractTableModel interface

	int			rowCount(const QModelIndex &parent = QModelIndex()) const override			
	{ 
		//if ( ! mbIsReady )
		//	return 0;
		//else
			return (int)mtabChannel[0].size(); 
	}

	int			columnCount(const QModelIndex &parent = QModelIndex()) const override		
	{ 
		return (int)mtabChannel.size(); 
	}

    QVariant	data(const QModelIndex &index, int role = Qt::DisplayRole) const override
	{
		if ( index.column() >= mtabChannel.size() )
			return QVariant();

		if ( index.row() >= mtabChannel[ index.column() ].size() )
			return QVariant();

		if (role == Qt::DisplayRole)
			return mtabChannel[index.column()][index.row()];

		return QVariant();
	}

	QVariant	headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

//------------------------------------------------------------------------------------------------//

public:
	void				initChannels();

	void				clear()
	{
		size_t n = mtabChannel.size();
		mtabChannel.clear();
		mtabChannel.resize( n);
	}

	size_t				sizeChannel() const					{ return mSizeCh; } //mtabChannel.size(); }
	size_t				size() const						{ return mtabChannel[0].size(); }

	bool				checkSizes() const;

	const DataChannel&	getChannel( size_t ich) const			{ return mtabChannel[ich]; }
	const ChannelRange&	getDataRange( size_t ich) const			{ return mtabChannel[ich].getRange(); }
	double				getData( size_t ich, size_t i) const	{ return mtabChannel[ich][i]; }


//	const Document*		getParentDoc() const	{ return mpParentDoc; }


	void				append( const RawData& data)
	{
		if ( data.size() != mtabChannel.size() )
		{
			qDebug() << "MeasurementData::append :: inconsistend sizes" << data.size() << mtabChannel.size();
			// error
		}

		for (size_t ich = 0; ich < mtabChannel.size(); ++ich)
			mtabChannel[ich].update( data[ich] );

		++mSizeData;

		assert( checkSizes() );

		emit this->layoutChanged();
	}

	//const QString&	cText() const		{ return mText; }
	//QString&		rText()				{ return mText; }

protected:
	void			setDefaults();

private:
	bool		mbIsReady;

	//QString		mText;

	size_t		mSizeCh;
	size_t		mSizeData;

	std::vector<DataChannel>	mtabChannel;    
//	std::vector<DataChannel>	mtabExtChannel;   // ???? 
};

QDataStream& operator<<( QDataStream& out, const MeasurementData& datm);
QDataStream& operator>>( QDataStream& in, MeasurementData& datm);



#endif // __MEASURE_DATA_H__
