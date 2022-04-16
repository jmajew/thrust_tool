#include "controller.h"

#include <QMessageBox.h>

#include "mainwindow.h"


Controller::Controller(MainWindow* parent) 
	: QObject(parent), mpMain(parent), mbIsOn(false), mIdBeg(-1), mIdCurPoint(-1), mpList(nullptr)
{
}

void Controller::ExecutePoint(const MsPoint& pnt)
{
}

void Controller::Execute(const AutoMeasureList& list)
{
}


void Controller::onStart( const AutoMeasureList* plist)
{
	if ( mbIsOn)
		return;

	const MeasurementData& data = mpMain->cDoc().cData();

	mpList = plist;
	mIdBeg = (int)data.size();	// mSizeData ???

	if ( mpList != nullptr)
	{
		if ( mpList->size() > 0 )
		{
			if ( mpMain->isArmed() )
			{
				// lock other forms
				// start execution
				mIdCurPoint = 0;
				mIdCur = mIdBeg;
				mpMain->startMeasurement();
				const MsPoint& pnt = (*mpList)[mIdCurPoint];

				qDebug() << "Controller :: motor level" << pnt[EMsPoint_Level];
				mpMain->startMotor( pnt[EMsPoint_Level] );
				mbIsOn = true;
				emit started();
			}
			else
			{
				qDebug() << "Controller :: motor is not armed";
			    QMessageBox::warning( nullptr, QString(tr("Warning")), QString(tr("Motor is not armed")) );
			}
		}
	}
}

void Controller::onStop()
{
	// stop execution
	mpMain->stopMotor();
	mpMain->stopMeasurement();

	mIdCurPoint = -1;
	mpList = nullptr;
	mbIsOn = false;
	emit stopped();

	// unlock other forms
}



void Controller::onDataReceived()
{
	if ( !mpList)
		return;

	int ibeg = mpList->getPntBegining( mIdCurPoint );
	if ( ibeg < 0)
		return;

	const MeasurementData& data = mpMain->cDoc().cData();
	size_t iend = data.size();
	
	int cur_level = data.getData( CH_EXT_MOTOR_OUT_ID, iend-1);

	//// CHECKME :: check the safeguards
	//double volt = data.getData( CH_EXT_BATT_VOLT_ID, iend-1);
	//int s = batteryNCellDetect(volt);
	//double cell_volt = volt / s;
	//qDebug() << "   Controller :: cell voltage = " << cell_volt;

	//if ( cell_volt < 3.4 )
	//{
	//	// stop
	//	onStop();
	//	return;
	//}

	//double thrust = data.getData( CH_EXT_THRUST_ID, iend-1);
	//double torque = data.getData( CH_EXT_TORQUE_ID, iend-1);
	//double curr = data.getData( CH_EXT_BATT_CURR_ID, iend-1);
	//double volt = data.getData( CH_EXT_BATT_VOLT_ID, iend-1);
	//
	//if ( thrust > 0.7 * 2000.0 || 
	//	 torque > 0.7 * 1000.0/3.0 ||
	//	 curr > 30.0 )
	//{
	//	// stop
	//	onStop();
	//	return;
	//}

	const MsPoint& pnt = (*mpList)[mIdCurPoint];
		qDebug() << "   Controller :: i = " << iend << ", " << ibeg << ", " << mIdBeg;

	if (iend - mIdBeg - ibeg > pnt[EMsPoint_Measure])
	{
		//if (mIdCurPoint > 0)
		//	if ( cur_level != (*mpList)[mIdCurPoint][EMsPoint_Level])
		//		return;

		++mIdCurPoint;

		if (mIdCurPoint >= mpList->size() )
		{
			// stop
			//mpMain->stopMotor();
			//mpMain->stopMeasurement();
			onStop();
			return;
		}
		else
		{
			int level = (*mpList)[mIdCurPoint][EMsPoint_Level];
			qDebug() << "   Controller :: motor level" << level;
			mpMain->startMotor( level );
		}
	}
}

