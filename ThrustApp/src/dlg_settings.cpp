#include <QLayout>

#include "dlg_settings.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormCfgTool
////////////////////////////////////////////////////////////////////////////////////////////////////
FormCfgTool::FormCfgTool(QWidget* parent) : QWidget(parent)
{
    setupUi(this);

    for (int i = 0; i < size_MotorProtocol; ++i)
        this->comboBox_MotorProt->addItem(QString(names_MotorProtocol[i]), i);

    for (int i = 0; i < size_StGaugeRate; ++i)
        this->comboBox_StGSmpls->addItem(QString(names_StGaugeRate[i]), i);

    for (int i = 0; i < size_StGaugeGain; ++i)
        this->comboBox_StGGain->addItem(QString(names_StGaugeGain[i]), i);

    comboBox_MotorProt->setEditable(false);
    comboBox_StGSmpls->setEditable(false);
    comboBox_StGGain->setEditable(false);

    //connect( spinNSamples, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FormCalibration::updateTime);
}

void FormCfgTool::initValues(const Config& cfg)
{
    //this->comboBox_MotorProt
    this->spinBox_MotorZero->setValue(cfg.groupMotor.zeroOutput);
    this->spinBox_MotorMin->setValue(cfg.groupMotor.minOutput);
    this->spinBox_MotorMax->setValue(cfg.groupMotor.maxOutput);

    this->spinBox_RpmFreq->setValue(cfg.groupRpm.smplFreq);
    this->spinBox_AdcSmpls->setValue(cfg.groupADC.nADCSamples);


    //this->comboBox_StGSmpls
    //this->comboBox_StGGain

    this->comboBox_MotorProt->setCurrentIndex(cfg.groupMotor.protocol);
    this->comboBox_StGSmpls->setCurrentIndex(cfg.groupStrain.sampleRate);
    this->comboBox_StGGain->setCurrentIndex(cfg.groupStrain.gain);
}

void FormCfgTool::fetchValues(Config& cfg)
{
    //this->comboBox_MotorProt
    cfg.groupMotor.zeroOutput = this->spinBox_MotorZero->value();
    cfg.groupMotor.minOutput = this->spinBox_MotorMin->value();
    cfg.groupMotor.maxOutput = this->spinBox_MotorMax->value();

    cfg.groupRpm.smplFreq = this->spinBox_RpmFreq->value();
    cfg.groupADC.nADCSamples = this->spinBox_AdcSmpls->value();


    //this->comboBox_StGSmpls
    //this->comboBox_StGGain

    cfg.groupMotor.protocol = (EMotorProtocol)this->comboBox_MotorProt->currentIndex();
    cfg.groupStrain.sampleRate = (EHX711Rate)this->comboBox_StGSmpls->currentIndex();
    cfg.groupStrain.gain = (EHX711Gain)this->comboBox_StGGain->currentIndex();
}

bool FormCfgTool::compareValues(const Config& cfg)
{
    if (this->spinBox_MotorZero->value() != cfg.groupMotor.zeroOutput )
        return false;
    else if (this->spinBox_MotorMin->value() != cfg.groupMotor.minOutput )
        return false;
    else if (this->spinBox_MotorMax->value() != cfg.groupMotor.maxOutput)
        return false;
    else if (this->spinBox_RpmFreq->value() != cfg.groupRpm.smplFreq)
        return false;
    else if (this->spinBox_AdcSmpls->value() != cfg.groupADC.nADCSamples)
        return false;
    else if (this->comboBox_MotorProt->currentIndex() != cfg.groupMotor.protocol)
        return false;
    else if (this->comboBox_StGSmpls->currentIndex() != cfg.groupStrain.sampleRate)
        return false;
    else if (this->comboBox_StGGain->currentIndex() != cfg.groupStrain.gain )
        return false;
    else
        return true;

    //if (this->spinBox_MotorZero->value() != cfg.groupMotor.zeroOutput ||
    //    this->spinBox_MotorMin->value() != cfg.groupMotor.minOutput ||
    //    this->spinBox_MotorMax->value() != cfg.groupMotor.maxOutput ||
    //    this->spinBox_RpmFreq->value() != cfg.groupRpm.smplFreq ||
    //    this->spinBox_AdcSmpls->value() != cfg.groupADC.nADCSamples ||
    //    this->comboBox_MotorProt->currentIndex() != cfg.groupMotor.protocol ||
    //    this->comboBox_StGSmpls->currentIndex() != cfg.groupStrain.sampleRate ||
    //    this->comboBox_StGGain->currentIndex() != cfg.groupStrain.gain )
    //{
    //    return false;
    //}

    //return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormCfgConvert
////////////////////////////////////////////////////////////////////////////////////////////////////
FormCfgConvert::FormCfgConvert(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    //connect( spinNSamples, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FormCalibration::updateTime);
}

void FormCfgConvert::initValues( const Config& cfg)
{
    // follows void DataConverter::init( const ToolSetup& setup)

    this->dspin_Vref->setValue( (double)cfg.groupConvert.Vref / 1000.0 );
	this->dspin_VbatDiv->setValue( (double)cfg.groupConvert.VbatDivider / 1000.0 );
	this->dspin_VbatCorr->setValue( (double)cfg.groupConvert.VbatCorr / 10000.0 );
	this->dspin_TroqueL->setValue( (double)cfg.groupConvert.LeverArmLength / 100.0 ); 	// <-- mm
	this->dspin_CurrCoef->setValue( (double)cfg.groupConvert.VIcoeff / 100.0 );

	this->dspin_ThrustCoef->setValue( (double)cfg.groupConvert.tabStGScale[0] / 1000. );
	this->dspin_TorqueCoef->setValue( (double)cfg.groupConvert.tabStGScale[1] / 1000. );

	this->dspin_ThrustOff->setValue( (double)cfg.groupConvert.tabStGZero[0] / 1000. );
	this->dspin_TorqueOff->setValue( (double)cfg.groupConvert.tabStGZero[1] / 1000. );
}

void FormCfgConvert::fetchValues( Config& cfg)
{
    // follows void DataConverter::init( const ToolSetup& setup)

    cfg.groupConvert.Vref = (int) ( this->dspin_Vref->value()* 1000.0 );
	cfg.groupConvert.VbatDivider = (int) (this->dspin_VbatDiv->value() * 1000.0 );
	cfg.groupConvert.VbatCorr = (int) ( this->dspin_VbatCorr->value() * 10000.0 );
	cfg.groupConvert.LeverArmLength = (int) (this->dspin_TroqueL->value() * 100.0 ); 	// <-- mm
	cfg.groupConvert.VIcoeff = (int) (this->dspin_CurrCoef->value() * 100.0 );

	cfg.groupConvert.tabStGScale[0] = (int) (this->dspin_ThrustCoef->value() * 1000. );
	cfg.groupConvert.tabStGScale[1] = (int) (this->dspin_TorqueCoef->value() * 1000. );

	cfg.groupConvert.tabStGZero[0] = (int) ( this->dspin_ThrustOff->value() * 1000. );
	cfg.groupConvert.tabStGZero[1] = (int) ( this->dspin_TorqueOff->value() * 1000. );
}

bool FormCfgConvert::compareValues( const Config& cfg)
{
    // follows void DataConverter::init( const ToolSetup& setup)

    //int u = (int) ( this->dspin_VbatCorr->value() * 10000.0);

    if (cfg.groupConvert.Vref != (int) ( this->dspin_Vref->value()* 1000.0) )
        return false;
    else if (cfg.groupConvert.VbatDivider != (int) (this->dspin_VbatDiv->value() * 1000.0) )
        return false;
    else if (cfg.groupConvert.VbatCorr != (int) ( this->dspin_VbatCorr->value() * 10000.0) )
        return false;
    else if (cfg.groupConvert.LeverArmLength != (int) (this->dspin_TroqueL->value() * 100.0) )
        return false;
    else if (cfg.groupConvert.VIcoeff != (int) (this->dspin_CurrCoef->value() * 100.0) )
        return false;

    else if (cfg.groupConvert.tabStGScale[0] != (int) (this->dspin_ThrustCoef->value() * 1000.) )
        return false;
    else if (cfg.groupConvert.tabStGScale[1] != (int) (this->dspin_TorqueCoef->value() * 1000.) )
        return false;

    else if (cfg.groupConvert.tabStGZero[0] != (int) ( this->dspin_ThrustOff->value() * 1000.) )
        return false;
    else if (cfg.groupConvert.tabStGZero[1] != (int)(this->dspin_TorqueOff->value() * 1000.) )
        return false;
    else
        return true;

    //if (cfg.groupConvert.Vref != (int) ( this->dspin_Vref->value()* 1000.0) ||
	   // cfg.groupConvert.VbatDivider != (int) (this->dspin_VbatDiv->value() * 1000.0) ||
	   // cfg.groupConvert.VbatCorr != (int) ( 10000.0 / this->dspin_VbatCorr->value()) ||
	   // cfg.groupConvert.LeverArmLength != (int) (this->dspin_TroqueL->value() * 100.0) ||
	   // cfg.groupConvert.VIcoeff != (int) (this->dspin_CurrCoef->value() * 100.0) ||

	   // cfg.groupConvert.tabStGScale[0] != (int) (this->dspin_ThrustCoef->value() * 1000.) ||
	   // cfg.groupConvert.tabStGScale[1] != (int) (this->dspin_TorqueCoef->value() * 1000.) ||

	   // cfg.groupConvert.tabStGZero[0] != (int) ( this->dspin_ThrustOff->value() * 1000.) ||
    //    cfg.groupConvert.tabStGZero[1] != (int)(this->dspin_ThrustOff->value() * 1000.) )
    //{
    //    return false;
    //}

    //return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// class DlgSettings
////////////////////////////////////////////////////////////////////////////////////////////////////
DlgSettings::DlgSettings( QWidget* parent) : QDialog(parent)
{
    mTab = new QTabWidget( this);

    parentWidget();

    mCfgToolTab = new FormCfgTool( this );
    mCfgConvertTab = new FormCfgConvert( this );

    mTab->addTab( mCfgToolTab, tr("Tool Config"));
    mTab->addTab( mCfgConvertTab, tr("Converter"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mTab);

    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    QSpacerItem* horizontalSpacer = new QSpacerItem(88, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

    buttonOk = new QPushButton(this);
    buttonOk->setObjectName(QString::fromUtf8("buttonOk"));

    buttonCancel = new QPushButton(this);
    buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));

    buttonOk->setText("OK");
    //buttonApply->setText(QCoreApplication::translate("DlgGraphStyle", "Apply", nullptr));
    buttonCancel->setText("Cancel");

    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(buttonOk);
    horizontalLayout->addWidget(buttonCancel);

    mainLayout->addLayout(horizontalLayout);


    setLayout(mainLayout);

    QObject::connect( buttonOk, SIGNAL(clicked()), this, SLOT(accept()) );
    QObject::connect( buttonCancel, SIGNAL(clicked()), this, SLOT(reject()) );
}

void DlgSettings::initValues( const Config& cfg)
{
    mCfgToolTab->initValues(cfg);
    mCfgConvertTab->initValues(cfg);
}

void DlgSettings::fetchValues( Config& cfg)
{
    mCfgToolTab->fetchValues(cfg);
    mCfgConvertTab->fetchValues(cfg);
}

bool DlgSettings::compareValues( const Config& cfg)
{
    if ( mCfgToolTab->compareValues(cfg) && mCfgConvertTab->compareValues(cfg) )
        return true;
    
    return false;
}
