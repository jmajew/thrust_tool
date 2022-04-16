#include <iostream>
#include <sstream>
#include <iomanip>

#include <QMenu>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QSettings>


#include "widget_control.h"
#include "measurement_setup.h"
#include "measure_data.h"
#include "dlg_autocreate.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormConnection
////////////////////////////////////////////////////////////////////////////////////////////////////

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

FormConnection::FormConnection(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    btnDisconnect->setDisabled( true);

	FillPortsInfo();

	comboRate->addItem( QStringLiteral("4800"), QSerialPort::Baud4800);
	comboRate->addItem( QStringLiteral("9600"), QSerialPort::Baud9600);
	comboRate->addItem( QStringLiteral("19200"), QSerialPort::Baud19200);
	comboRate->addItem( QStringLiteral("38400"), QSerialPort::Baud38400);
	comboRate->addItem( QStringLiteral("57600"), QSerialPort::Baud57600);
	comboRate->addItem( QStringLiteral("115200"), QSerialPort::Baud115200);
	comboRate->addItem( QStringLiteral("250000"), QVariant(250000));
	comboRate->addItem( QStringLiteral("500000"), QVariant(500000));
	comboRate->addItem( QStringLiteral("1000000"), QVariant(1000000));
    comboRate->setCurrentIndex(7);

    ShowPortInfo( 1);

	//connect( ui.checkBoxUsePerspective, SIGNAL( stateChanged(int) ), this, SLOT( togglePerspective(int) ) );
	//connect( ui.doubleSpinBoxViewAngle, SIGNAL( valueChanged(double) ), this, SLOT( setViewAngle(double) ) );

    connect( comboPort, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FormConnection::ShowPortInfo);

    connect( btnRescan, &QPushButton::released, this, &FormConnection::FillPortsInfo);

    labPortDescription->setMinimumWidth( 100);
    //comboPort->setMaximumWidth( 100);
    //comboRate->setMaximumWidth( 100);

	//readSettings();

}

void FormConnection::FillPortsInfo()
{
    comboPort->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;

    comboPort->clear();

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) 
	{
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        QString str = list.first();
        comboPort->addItem( list.first(), list);
    }
}

void FormConnection::ShowPortInfo( int idx)
{
    if (idx == -1)
        return;

    QStringList list = comboPort->itemData(idx).toStringList();
    labPortDescription->setText(tr("%1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));

    //ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    //ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    //ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    //ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    //ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormSetup
////////////////////////////////////////////////////////////////////////////////////////////////////
FormSetup::FormSetup(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    //connect( spinNSamples, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FormCalibration::updateTime);
}

void FormSetup::InitValues( const MeasurementSetup& setup)
{
    this->editMotorName->setText( setup.cMotorName() );
    this->spinMotorKV->setValue( setup.cMotorKV() );
    this->spinMotorPolesCount->setValue( setup.cMotorPoles() );
    this->spinBladeCount->setValue( setup.cPropBlades() );
    
    this->checkBox_CurrZero->setChecked( setup.cCurZeroEnabled() );
    this->checkBox_EscTelem->setChecked( setup.cEscTelemEnabled() );

    if ( setup.cRpmSource() == ERpmSource::IrSensor )
        this->radioButton_Sensor->setChecked( true);
    else if ( setup.cRpmSource() == ERpmSource::EscTelem )
        this->radioButton_Telem->setChecked( true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormMeasurement
////////////////////////////////////////////////////////////////////////////////////////////////////
FormMeasurement::FormMeasurement(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    //connect( spinNSamples, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FormCalibration::updateTime);
}




////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormProcess
////////////////////////////////////////////////////////////////////////////////////////////////////
FormProcess::FormProcess(QWidget *parent)
{
    setupUi(this);
}




////////////////////////////////////////////////////////////////////////////////////////////////////
// class ControlWidget
////////////////////////////////////////////////////////////////////////////////////////////////////

ControlWidget::ControlWidget(QWidget *parent) 
    : QWidget(parent) 
{
    mBox = new QToolBox( this);

    mFormConnect = new FormConnection(mBox);
    mBox->addItem( mFormConnect, tr("Connection"));

    mFormSetup = new FormSetup(this);
    mBox->addItem( mFormSetup, tr("Measurement Setup"));
    mFormSetup->setDisabled(true);

    //mFormCalibrate = new FormCalibration(this);
    //mBox->addItem( mFormCalibrate, tr("Calibration"));
    //mFormCalibrate->setDisabled(true);

    mFormMeasurement = new FormMeasurement(this);
    mBox->addItem( mFormMeasurement, tr("Manual Measurement"));
    mFormMeasurement->setDisabled(true);

    mFormAutoMeasure = new FormAutoMeasurement(this);
    mBox->addItem( mFormAutoMeasure, tr("Auto Measurement"));
    mFormAutoMeasure->setDisabled(true);
    
    mFormProcess = new FormProcess(this);
    mBox->addItem( mFormProcess, tr("Process"));
    //mFormProcess->setDisabled(true);

    //QWidget *ptr;
    //mBox->addItem( ptr = new QWidget(mBox), tr("Auto Measurement"));
    //ptr->setDisabled(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mBox);
    setLayout(mainLayout);
}

void ControlWidget::linkMainWindow( MainWindow* pwin)
{
    //mFormCalibrate->linkMainWindow( pwin);
}


void ControlWidget::enableForms()
{
    //mFormCalibrate->setEnabled(true);
    mFormSetup->setEnabled(true);
    mFormMeasurement->setEnabled(true);
    mFormAutoMeasure->setEnabled(true);
    mFormProcess->setEnabled(true);
}

void ControlWidget::disableForms()
{
    //mFormCalibrate->setDisabled(true);
    mFormSetup->setDisabled(true);
    mFormMeasurement->setDisabled(true);
    mFormAutoMeasure->setDisabled(true);
    //mFormProcess->setDisabled(true);
}
