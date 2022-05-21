#include <iostream>
#include <fstream>
#include <iomanip>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QMessageBox>
#include <QSettings>

#include "document.h"
#include "latex_generator.h"


const uint32_t	Document::mVer			= 3;
const uint32_t	Document::mMagickKey	= 0xA1B2C3E4;




////////////////////////////////////////////////////////////////////////////////////////////////////
// class DocStyle
////////////////////////////////////////////////////////////////////////////////////////////////////

QDataStream& operator<<( QDataStream& out, const DocStyle& head)
{
	return out;
}

QDataStream& operator>>( QDataStream& in, DocStyle& head)
{
	return in;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
// class DocHeader
////////////////////////////////////////////////////////////////////////////////////////////////////


QDataStream& operator<<( QDataStream& out, const DocHeader& head)
{
	out << head.mApiVersion;
	out << head.mBoardInfo;
	out << head.mSoftInfo;
	out << head.mBuildInfo;

	return out;
}

QDataStream& operator>>( QDataStream& in, DocHeader& head)
{
	//QString str;
	//in >> str;	
	//head.setApiVer(THSP_PROTOCOL_VERSION, API_VERSION_MAJOR, API_VERSION_MINOR);
	in >> head.mApiVersion;
	in >> head.mBoardInfo;
	in >> head.mSoftInfo;
	in >> head.mBuildInfo;

	return in;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class Document
////////////////////////////////////////////////////////////////////////////////////////////////////

Document::Document( QObject *parent) : QObject(parent), mConverter(this), mpMsData(nullptr)
{
	mpMsData = new MeasurementData( this);
	
	QSettings settings;
    QString key_root = "Document/";

    mTmplName = settings.value(key_root + "TemplateName").toString();
	//if ( mTmplName == "" )
	//mTmplName = "tmpl_2.tex";
}

Document::~Document()
{
	QSettings settings;
    QString key_root = "Document/";

	settings.setValue( key_root + "TemplateName", mTmplName );
	
	if ( mpMsData )
		delete mpMsData;
}


void Document::Reset()
{
	mHeader.Reset();
	mStyle.Reset();
	mSetup.setDefaults();
	//mResult.Reset();

	if ( mpMsData )
		delete mpMsData;

	mpMsData = new MeasurementData( this);
}


void Document::Load( const QString& fname)
{
	QFile file(fname);
	if ( !file.open(QIODevice::ReadOnly) )
		return;

	mFileName = fname;

	uint32_t key;
	QDataStream fin(&file);
	
	fin >> key;
	if ( key != mMagickKey )
	{
		// not a correct document file
		// TODO :: show a warning !
		return;
	}

	fin >> key;
	if ( key != mVer )
	{
		// wrong version of the file
		qDebug() << "wrong ver. of the file '" << key << ", - required '" << mVer << "'";
		QMessageBox::warning( nullptr,	tr("Warning"), tr("Wrong file version") );
		return;
	}

	this->Reset();

	fin >> mHeader;
	fin >> mConverter;
	fin >> mSetup;
	//fin >> mResult;

	if ( mpMsData != nullptr )
		delete mpMsData;

	// CHECKME :: what if data (during saving) was nullptr ?
	mpMsData = new MeasurementData(this);
	fin >> *mpMsData;


	//emit dataChanged();
}


void Document::Save( const QString& fname)
{
	QFile file(fname);
	if ( !file.open(QIODevice::WriteOnly) )
		return;

	mFileName = fname;

	QDataStream fout(&file);
	fout << mMagickKey;
	fout << mVer;

	fout << mHeader;
	fout << mConverter;
	fout << mSetup;
	//fout << mResult;

	if ( mpMsData != nullptr )
		fout << *mpMsData;
}

void Document::GenerateReport( const QString& fname)
{
	QFileInfo finfo(fname);
	QString ext = finfo.suffix();
	if ( ext == "tex" )
	{
		GenerateReport_LATEX( fname);
	}
	else if ( ext == "txt" )
	{
		GenerateReport_TXT( fname);
	}
	else
	{
		// unrecognized extension
		QMessageBox::warning( nullptr,	tr("Warning"), tr("Unrecognized report extension") );
	}
}

void Document::GenerateReport_TXT( const QString& fname)
{
	QFile file(fname);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug() <<"can not open the file: " << fname;
		QMessageBox::warning( nullptr,	("Warning"), ("TextGenerator: Can't open the output file:") + fname );
        return;
	}

    QTextStream out(&file);

	out << "## Tool info\n";
	out << "Serial API: " << mHeader.strApiVer() << "\n";
	out << "Board:      " << mHeader.strBoardInfo() << "\n";
	out << "Firmware:   " << mHeader.strSoftInfo() << "\n";
	out << "Commit:     " << mHeader.strBuildInfo() << "\n";
    out << "\n\n";

	out << "## Measurement info\n";
	out << "Motor :        " << mSetup.cMotorName() << "\n";
	out << "KV :           " << mSetup.cMotorKV() << "\n";
	out << "Magnet poles : " << mSetup.cMotorPoles() << "\n\n";
	out << "Propeler:      " << mSetup.cPropName() << "\n";
	out << "Blade number : " << mSetup.cPropBlades() << "\n";
    out << "\n\n";

	mResult.WriteData( out, mpMsData );
}

void Document::WriteToolInfo_LATEX( std::ofstream& fout) const
{
	fout << "Serial API: " << " & " << mHeader.strApiVer().toStdString() << "\\\\ \n";
	fout << "Board:      " << " & " << mHeader.strBoardInfo().toStdString() << "\\\\ \n";
	fout << "Firmware:   " << " & " << mHeader.strSoftInfo().toStdString() << "\\\\ \n";
	fout << "Commit:     " << " & " << mHeader.strBuildInfo().toStdString() << "\n";
}


void Document::WriteSetupInfo_LATEX( std::ofstream& fout) const
{
	fout << "Motor:       " << " & " << mSetup.cMotorName().toStdString() << "\\\\ \n";
	fout << "KV:          " << " & " << mSetup.cMotorKV() << "\\\\ \n";
	fout << "Magnet poles:" << " & " << mSetup.cMotorPoles() << "\\\\ \n";
	fout << "Propeler:    " << " & " << mSetup.cPropName().toStdString() << "\\\\ \n";
	fout << "Blade count: " << " & " << mSetup.cPropBlades() << "\\\\ \n";
	fout << "Note:        " << " & " << mSetup.cComment().toStdString() << "\n";
}

void Document::GenerateReport_LATEX( const QString& fname)
{
	const QString tmpl_name = "";

	LatexGenerator<Document> gen( *this);
	gen.SetTmplName( this->mTmplName.toStdString() );
	gen.Execute( fname.toStdString() );

}


void Document::appendData( ThspMessage& msg)
{
	SBuf	sbin( msg.data);

//----------------------------------------------------
// TODO :: update raw channels

	// data frame id
	uint32_t msg_id = sbin.ReadU32();
	RawData data( msg_id);

	// motor
	data[CH_MOTOR_OUT_ID]		= static_cast<double>( sbin.ReadU16() );

	// strain gauge
	uint8_t  nsg = STGAUGE_CH_COUNT;

	for ( int i=0; i< nsg; ++i)
		data[CH_STG_THRUST_ID + i] = static_cast<double>( (int32_t)sbin.ReadU32() );

	// adc
	uint8_t  nadc = ADC_SENS_DATA_CH_COUNT;

	//StatValueU16 sval;
	for ( int i=0; i< nadc; ++i)
	{
		data[CH_ADC_VREF_ID + i] = static_cast<double>( sbin.ReadU16() );

		//sval.n   =	sbin.ReadU16();
		//sval.sx  =	sbin.ReadU32();
		//sval.sx2 =	sbin.ReadU64();

		//data[CH_ADC_VREF_ID + i] = static_cast<double>( sval.Mean() );
	}

	// rpm
	data[CH_RPM_ID] = static_cast<double>( sbin.ReadU16() );

	// esc telemetry
	data[CH_ESCT_VBATT_ID]	= static_cast<double>( sbin.ReadU16() );
	data[CH_ESCT_IBATT_ID]	= static_cast<double>( sbin.ReadU16() );
	data[CH_ESCT_RPM_ID]	= static_cast<double>( sbin.ReadU16() );
	data[CH_ESCT_TEMP_ID]	= static_cast<double>( sbin.ReadU8() );		// TODO :: should be signed int ?

//----------------------------------------------------
// TODO :: update extended/cooked channels

	mConverter.convert( data );

	mpMsData->append( data ); // FIXME
}


//void Document::resetMeasurement()	
//{
//	//mcurMsId = mMsData.size();
//	//mMsData.push_back( new MeasurementData( mcurMsId) );
//}
