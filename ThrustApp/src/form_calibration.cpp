#include <QMessageBox>
#include <QMenu>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "form_calibration.h"
#include "mainwindow.h"
#include "thsp_packet.hpp"



////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormCalibration
////////////////////////////////////////////////////////////////////////////////////////////////////
FormCalibration::FormCalibration(QWidget *parent)
    : QWidget(parent), mpWin(nullptr)
{
    setupUi(this);

    //lineEditThrust->
    doubleThrust->setValue(0.);
    doubleTorque->setValue(0.);
    doubleTorque->setDisabled(true);


    mType = ECalibType::Thrust;

    tableWidget->setColumnCount( 2);
    tableWidget->setRowCount( 0);

	QHeaderView *verticalHeader = tableWidget->verticalHeader();
	verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
	verticalHeader->setDefaultSectionSize(20);

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	QStringList labels;
	labels << tr("Load") << tr("Value");
	tableWidget->setHorizontalHeaderLabels(labels);
	tableWidget->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch);
	tableWidget->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Stretch);
	//tableWidget->setColumnWidth(0, 95);
	//tableWidget->setColumnWidth(1, 95);


    connect( spinNSamples, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FormCalibration::updateTime);

    connect( radioThrust, &QRadioButton::toggled, this, &FormCalibration::onRadioThrust);
    connect( radioTorque, &QRadioButton::toggled, this, &FormCalibration::onRadioTorque);

    connect( btnExecute, &QPushButton::clicked, this, &FormCalibration::onExecute );
    connect( btnAddPoint, &QPushButton::clicked, this, &FormCalibration::onAddPoint );
    connect( btnCalcCoeff, &QPushButton::clicked, this, &FormCalibration::onCalc );

    connect( tableWidget, &QTableWidget::customContextMenuRequested, this, &FormCalibration::onContextMenu );

}


void FormCalibration::onContextMenu(const QPoint& pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Delete row", this);

    connect( &action1, &QAction::triggered, this, &FormCalibration::onDeleteRow );
   
    contextMenu.addAction(&action1);

    contextMenu.exec(tableWidget->mapToGlobal(pos));
    //contextMenu.exec(pos);
}

void FormCalibration::onDeleteRow()
{
	QModelIndexList selection = tableWidget->selectionModel()->selectedRows();

	for( int i=0; i< selection.count(); ++i)
	{
		int id = selection.at(i).row();
        tableWidget->removeRow( id);
        if ( id < mtabPnts.size() )
            mtabPnts.erase( mtabPnts.begin() + id );
    }

    tableWidget->clearSelection();
}


void FormCalibration::updateTime( int n)
{
    double freq = 10.0;
    double time = (double)n / freq;

    std::stringstream ss;  
    ss << std::setprecision( 3 );  
    ss << (double)time;

    labTime->setText( QString( ss.str().c_str() ) + QString(" sec") );
}

void FormCalibration::onRadioThrust( bool bcheck)
{
    if ( bcheck)
    {
        if ( mType != ECalibType::Thrust )
        {
            if ( QMessageBox::Yes == QMessageBox::question(this,"Calibration Type", "Are you sure switch to Thrust calibration?") )
            {
                mType = ECalibType::Thrust;
                doubleThrust->setEnabled(true);
                doubleTorque->setDisabled(true);
            }
            else
            {
                radioThrust->setChecked(false);
                radioTorque->setChecked(true);
            }
        }
    }
}

void FormCalibration::onRadioTorque( bool bcheck)
{
    if ( bcheck)
    {
        if ( mType != ECalibType::Torque )
        {
            if ( QMessageBox::Yes == QMessageBox::question(this,"Calibration Type", "Are you sure switch to Torque calibration?") )
            {
                mType = ECalibType::Torque;
                doubleThrust->setDisabled(true);
                doubleTorque->setEnabled(true);
            }
            else
            {
                radioThrust->setChecked(true);
                radioTorque->setChecked(false);
            }
        }
    }
}


void FormCalibration::onExecute()
{
    if ( mpWin == nullptr )
    {
        qDebug() << "FormCalibration::onExecute :: mpWin == nullptr";
        return;
    }

    uint16_t smpl_count = spinNSamples->value();

    if ( smpl_count <= 0 )
    {
        QMessageBox::information( this, "Warning", "Number of samples must be greater than 0 !" );
        return;
    }

    if ( mType == ECalibType::Thrust )
    {
        mLoad = doubleThrust->value();
    }
    else
    {
        mLoad = doubleTorque->value();
    }

    if ( ! mpWin->isSerialActive() )
    {
        qDebug() << "FormCalibration::onExecute :: serial not active !!";
        return;
    }

    mstatThrust.Reset();
    mstatTorque.Reset();

    mpWin->start_calib( smpl_count);

    //mpWin->send_cmnd( THSP_MEASURE_START );
    //mpWin->send_cmnd( THSP_DATASTREAM_START );
    //mpWin->send_cmnd( THSP_CALIBRATE_ZERO_START, smpl_count );

    ////mpWin->send_cmnd( THSP_CALIBRATE_ZERO_STOP );
    ////mpWin->send_cmnd( THSP_DATASTREAM_STOP );
    ////mpWin->send_cmnd( THSP_MEASURE_STOP );

    //mpWin->send_cmnd( THSP_GET_DATA_ZERO );
}

void FormCalibration::finishExecute()
{
    if ( mType == ECalibType::Thrust )
    {
        this->labelMean->setText( QString::number(mstatThrust.Mean()) );
        this->labelDeviat->setText( QString::number(mstatThrust.Deviation()) );
    }
    else
    {
        this->labelMean->setText( QString::number(mstatTorque.Mean()) );
        this->labelDeviat->setText( QString::number(mstatTorque.Deviation()) );
    }
}

void FormCalibration::updateExecute( int32_t thrust, int32_t torque)
{
    mstatThrust.Accumulate( thrust);
    mstatTorque.Accumulate( torque);

    if ( mType == ECalibType::Thrust )
    {
        this->labelMean->setText( QString::number(mstatThrust.Mean()) );
        this->labelDeviat->setText( QString::number(mstatThrust.Deviation()) );
    }
    else
    {
        this->labelMean->setText( QString::number(mstatTorque.Mean()) );
        this->labelDeviat->setText( QString::number(mstatTorque.Deviation()) );
    }
}


void FormCalibration::onAddPoint()
{
    if ( mstatThrust.Count() <= 0 || mstatTorque.Count() <= 0)
        return;

    if ( mType == ECalibType::Thrust )
    {
        mtabPnts.emplace_back( mstatThrust.Mean(), mLoad );
    }
    else
    {
        mtabPnts.emplace_back( mstatTorque.Mean(), mLoad );
    }

    tableWidget->setRowCount( tableWidget->rowCount() + 1 );
    tableWidget->setItem( tableWidget->rowCount() - 1, 0, new QTableWidgetItem( QString::number( mtabPnts.back().first,'g', 8) ));
    tableWidget->setItem( tableWidget->rowCount() - 1, 1, new QTableWidgetItem( QString::number( mtabPnts.back().second,'g', 8) ));
}

void FormCalibration::onCalc()
{
    double sx2 = 0.;
    double sx = 0.;
    double sxy = 0.;
    double sy = 0.;
    double s1 = mtabPnts.size();

    for (int i = 0; i < mtabPnts.size(); ++i)
    {
        const double& x = mtabPnts[i].first;
        const double& y = mtabPnts[i].second;
        sx2 += x*x;
        sx += x;
        sxy += x*y;
        sy += y;
    }

    double d = sx2*s1 - sx*sx;
    double d1 = sxy*s1 - sx*sy;
    double d2 = sx2*sy - sxy*sx;

    double a = d1 / d;
    double b = d2 / d;

    this->labelCoeff->setText( QString::number( a ) );
    this->labelOffset->setText( QString::number( b ) );
}
