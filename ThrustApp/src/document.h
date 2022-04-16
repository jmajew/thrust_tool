#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <array>
#include <vector>

#include <QDebug>
#include <QtGlobal>
#include <QObject>
#include <QAbstractTableModel>

#include "thsp_packet.hpp"
#include "measurement_setup.h"
#include "measure_data.h"
#include "data_converter.h"
#include "result.h"

#include "config.hpp"



////////////////////////////////////////////////////////////////////////////////////////////////////
// class DocHeader
////////////////////////////////////////////////////////////////////////////////////////////////////
class DocHeader
{
	friend QDataStream& operator<<( QDataStream& out, const DocHeader& head);
	friend QDataStream& operator>>( QDataStream& in, DocHeader& head);

public:
	void	Reset()
	{
		mApiVersion = mBoardInfo = mSoftInfo = mBuildInfo = "";
	}

	void	setApiVer( uint8_t prot_ver, uint8_t api_major, uint8_t api_minor )
	{ 
		mApiVersion = QString::number(prot_ver) + ":" + QString::number(api_major) + "." + QString::number(api_minor); 
	}

	void	setBoardInfo( const QString& str)	{ mBoardInfo = str; }
	void	setSoftInfo( const QString& str)	{ mSoftInfo = str; }
	void	setBuildInfo( const QString& str)	{ mBuildInfo = str; }



	const QString&	strApiVer() const			{ return mApiVersion; }
	const QString&	strBoardInfo() const		{ return mBoardInfo; }
	const QString&	strSoftInfo() const			{ return mSoftInfo; }
	const QString&	strBuildInfo() const		{ return mBuildInfo; }

private:
	//boardinfo
	QString			mApiVersion;
	QString			mBoardInfo;
	QString			mSoftInfo;
	QString			mBuildInfo;
};

QDataStream &operator<<( QDataStream &, const DocHeader &);
QDataStream &operator>>( QDataStream &, DocHeader &);


////////////////////////////////////////////////////////////////////////////////////////////////////
// class DocStyle
////////////////////////////////////////////////////////////////////////////////////////////////////
class DocStyle
{
public:
	void	Reset()	{}
};

QDataStream &operator<<( QDataStream &, const DocStyle &);
QDataStream &operator>>( QDataStream &, DocStyle &);


template <class Generator>
class LatexGenerator;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class Document
////////////////////////////////////////////////////////////////////////////////////////////////////
class Document : public QObject
{
    Q_OBJECT

	friend LatexGenerator<Document>;

public:
	Document( QObject *parent = nullptr );
	~Document();

	void	Reset();
	void	Load( const QString& fname);
	void	Save( const QString& fname);

	void	GenerateReport( const QString& fname);

	const QString&			cTmplName() const	{ return mTmplName; }			
	QString&				rTmplName()			{ return mTmplName; }

	const DocHeader&		cHeader() const		{ return mHeader; }			
	DocHeader&				rHeader()			{ return mHeader; }

	const DocStyle&			cStyle() const		{ return mStyle; }			
	DocStyle&				rStyle()			{ return mStyle; }

	const MeasurementSetup&	cSetup() const		{ return mSetup; }			
	MeasurementSetup&		rSetup()			{ return mSetup; }

	const MeasurementData&	cData() const		{ ASSERT(mpMsData); return *mpMsData; }			
	MeasurementData&		rData()				{ ASSERT(mpMsData); return *mpMsData; }

	const ResultSet&		cResult() const		{ return mResult; }			
	ResultSet&				rResult()			{ return mResult; }

	const DataConverter&	cConverter() const	{ return mConverter; }

	void	initConverter( const ToolSetup& setup)	
	{ 
		mConverter.init( mSetup, setup); 
	}

	void	appendData( ThspMessage& msg);

private:
	void	GenerateReport_TXT( const QString& fname);
	void	GenerateReport_LATEX( const QString& fname);

//public:
	void	WriteToolInfo_LATEX( std::ofstream& fout) const;
	void	WriteSetupInfo_LATEX( std::ofstream& fout) const;


signals:

public slots:
	//void	resetMeasurement();

private:

	static const uint32_t	mVer;
	static const uint32_t	mMagickKey;

	QString				mTmplName;

	// to save/load
	DocHeader			mHeader;
	DocStyle			mStyle;
	DataConverter		mConverter;

	MeasurementSetup	mSetup;
	MeasurementData*	mpMsData;

	ResultSet				mResult;
	//std::vector<Result>	mtabResult;
};



#endif // __DOCUMENT_H__

