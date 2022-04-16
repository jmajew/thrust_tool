#include <QtWidgets>

#include <QSplitter>
#include <QListView>
#include <QTreeView>
#include <QTextEdit>
#include <QLayout>
#include <QPushButton>
#include <QDebug>

#include "mainwindow.h"
#include "dlg_settings.h"





////////////////////////////////////////////////////////////////////////////////////////////////////
// class MainWindow
////////////////////////////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), mthSerial(nullptr), mAutoControl(this), mHBeatTimer(nullptr), mbHBeat(false)
{
    this->setStyleSheet(R"( QMainWindow::separator { background-color: rgb(215, 215, 215); } )" );

    mSerialStat = ESerialStatus::Closed;


//===========================================================
// actions 

    //const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    //QAction *newAct = new QAction(newIcon, tr("&New"), this);

    QAction* newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    QAction* openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);

    QAction* saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);


    QAction* selTmplAct = new QAction(tr("Select Latex Template"), this);
    //exportAct->setShortcuts(QKeySequence::Save);
    selTmplAct->setStatusTip(tr("Select template for Latex report"));
    connect(selTmplAct, &QAction::triggered, this, &MainWindow::select_export_tmpl);

    QAction* exportAct = new QAction(tr("&Export"), this);
    //exportAct->setShortcuts(QKeySequence::Save);
    exportAct->setStatusTip(tr("Export the measurement report to disk"));
    connect(exportAct, &QAction::triggered, this, &MainWindow::export_rep);

    QAction* exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    QAction* aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    QAction* settingsAct = new QAction(tr("&Settings"), this);
    settingsAct->setStatusTip(tr("Show the Settings Dialog"));
    connect(settingsAct, &QAction::triggered, this, &MainWindow::settings);

    // create menu 
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(selTmplAct);
    fileMenu->addAction(exportAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);

    // tool bar
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(settingsAct);

//===========================================================
// status bar

	//statusBar()->addPermanentWidget( new QPushButton("push") );
    statusBar()->showMessage(tr("Ready"));

    labVBatCell = new QLabel(tr("-.--V"), this);
    statusBar()->addPermanentWidget(labVBatCell);

    labMotorStatus = new QLabel(tr("Disarmed"), this);
    statusBar()->addPermanentWidget(labMotorStatus);
 
    labConnStatus = new QLabel(tr("Disconnected"), this);
    statusBar()->addPermanentWidget(labConnStatus);

    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), SLOT(tick()));
    //timer->start(500);


//    connect( act, &QAction::triggered, mControl, [=]() { mControl->mBox->setCurrentIndex(1); });
//    connect( act, &QAction::triggered, mControl, [=]() { QMessageBox::about(this, "Hello msg", "Hello!"); });
    


//===========================================================
// control dock widget 

    QDockWidget *dock = new QDockWidget( tr("Control Panel"), this);
	dock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
    //dock->setMinimumWidth( 250);

	mControl = new ControlWidget( dock);
    mControl->linkMainWindow( this);
    mControl->mFormSetup->InitValues( mDoc.cSetup() );

	mControl->setMinimumSize( 280, 400);
    dock->setWidget( mControl);
	addDockWidget( Qt::LeftDockWidgetArea, dock);

    dock->setStyleSheet("QDockWidget:title { background-color: rgb(220, 220, 220);}" );
    //self.dockWidget.setStyleSheet("QDockWidget:title {background-color: none;}")

//actions
    QAction* connAct = new QAction(tr("&Connect"), this);
    ////openAct->setShortcuts(QKeySequence::Open);
    connAct->setStatusTip(tr("Open connection"));
    fileToolBar->addAction(connAct);

    discAct = new QAction(tr("&Disconnect"), this);
    ////openAct->setShortcuts(QKeySequence::Open);
    discAct->setStatusTip(tr("Close connection"));
    fileToolBar->addAction(discAct);
    discAct->setDisabled(true);

    connect( connAct, &QAction::triggered, mControl, [=]() { mControl->mBox->setCurrentWidget(mControl->mFormConnect); } );
    connect( discAct, &QAction::triggered, this, &MainWindow::stop_connection );

     //mControl->mBox->setCurrentWidget(  mControl->mFormConnect );
     // 

    armAct  = new QAction(tr("Arm"), this);
    armAct->setStatusTip(tr("Arm motor"));
    fileToolBar->addAction(armAct);
    armAct->setCheckable(true);
    armAct->setDisabled(true);
    mbackArm = false;

    connect( armAct, &QAction::triggered, this, &MainWindow::arm );

//===========================================================
// output  

    //this->setCentralWidget( splitter);
    mOutput = new TabWidget( &mDoc, this);
    //mOutput->mDataTab->setModel( &mDoc.rData() );
    //mOutput->mGraphTab->reinitData( &mDoc.rData() );

	mOutput->setMinimumSize( 350, 300);
    this->setCentralWidget( mOutput);


    // connect / disconnect
    connect( mControl->mFormConnect->btnConnect, &QPushButton::released, this, &MainWindow::start_connection );
    connect( mControl->mFormConnect->btnDisconnect, &QPushButton::released, this, &MainWindow::stop_connection );

    // serial com
    connect( &mthSerial, &SerialThread::connected, this, &MainWindow::connected);
    connect( &mthSerial, &SerialThread::response, this, &MainWindow::receive);
    connect( &mthSerial, &SerialThread::error, this, &MainWindow::errorHandler);
    connect( &mthSerial, &SerialThread::timeout, this, &MainWindow::timeoutHandler);

    // debug_tab
    connect( mOutput->mDebugTab->btnSend, &QPushButton::released, this, &MainWindow::send );

    // terminal_tab
    connect( mOutput->mTermTab->mpbtnOpenTerm, &QPushButton::released, this, [=]() { this->stopHBeat(); this->send_cmnd( THSP_SHELL_START); } );
    connect( &mthSerial, &SerialThread::start_terminal, this, &MainWindow::start_terminal);
    connect( mOutput->mTermTab, &TerminalView::restoreSerial, this, &MainWindow::stop_terminal );
    //connect( &mthSerial, &SerialThread::start_terminal, this->mOutput->mTermTab, &TerminalView::enableTerminal);
    //connect( mOutput->mTermTab, &TerminalView::restoreSerial, this, [=]() { this->mthSerial.terminalStopped(); } );

    // form setup
    connect( mControl->mFormSetup->btnSave, &QPushButton::released, this, &MainWindow::applySetup );
    
    // form measurement
    connect( mControl->mFormMeasurement->btnMeasureStart, &QPushButton::released, this, [=]() { this->send_cmnd( THSP_DATASTREAM_START); } );
    connect( mControl->mFormMeasurement->btnMeasureStop, &QPushButton::released, this, [=]() { this->send_cmnd( THSP_DATASTREAM_STOP ); } );

    // TODO :: check if motor is armed
    connect( mControl->mFormMeasurement->btnMotorStart, &QPushButton::released, 
             this, [=]() { this->send_cmnd( THSP_MOTOR_SET_COMMAND, mControl->mFormMeasurement->spinMotorOutput->value()); } );
    connect( mControl->mFormMeasurement->btnMotorStop, &QPushButton::released, this, [=]() { this->send_cmnd( THSP_MOTOR_STOP); } );


        //connect( mOutput->mGraphTab, &GraphView::resetMeasurement, &mDoc, &Document::resetMeasurement);

    //connect( this, &MainWindow::dataChanged, this->mOutput, &TabWidget::onDataChenged);
    //connect( this->mOutput->mTermTab->mpConsole, &Console::resetSent, this, &MainWindow::onDeviceReset);


    //connect( mControl->mFormMeasurement->btnMotorStart, &QPushButton::released, this->mAutoControl, &Controller::onStart );
    //connect( this->mControl->mFormAutoMeasure->btnMeasureStart, &QPushButton::released, &this->mAutoControl, &Controller::onStart );
    connect( this->mControl->mFormAutoMeasure->btnMeasureStart, &QPushButton::released, 
            this, [=]() { this->mAutoControl.onStart( &mControl->mFormAutoMeasure->cList() ); } );

    connect( this->mControl->mFormAutoMeasure->btnMeasureStop, &QPushButton::released, 
            &this->mAutoControl, &Controller::onStop );

    connect( this, &MainWindow::dataReceived, &this->mAutoControl, &Controller::onDataReceived );

    connect( this->mControl->mFormProcess->pushButton_Process, &QPushButton::released, this, [=]() { this->onProcess( &mDoc.cData() ); } );
}


MainWindow::~MainWindow()
{
}


void MainWindow::start_terminal()
{
    //QQueue<ThspMessage> queMsg;
    //mthSerial.getReply( queMsg);

    //this->stopHBeat();
    this->mOutput->mTermTab->enableTerminal();
}

void MainWindow::stop_terminal()
{
    this->mthSerial.terminalStopped();
    this->startHBeat();
}

void MainWindow::onDeviceReset()
{
	qDebug() << "MainWindow ::  'reset' captered !!!";
}

void MainWindow::onProcess( const MeasurementData* data)
{
    int ibeg = 0;
    int iend = (int)data->size();

    mAnalyzer.SetMargins( mControl->mFormProcess->spinBox_Beg->value(), 
                          mControl->mFormProcess->spinBox_End->value() );

    this->mControl->mFormProcess->pushButton_Process->setDisabled(true);

    mAnalyzer.DoAnalyze( mDoc.rResult(), std::make_pair(ibeg, iend), data);
    
    this->mControl->mFormProcess->pushButton_Process->setEnabled(true);
}

void MainWindow::newFile()
{
    mDoc.Reset();

    mOutput->mDataTab->setModel( &mDoc.rData() );
    mOutput->mGraphTab->reinitData( &mDoc.rData() );
}

void MainWindow::open()
{
    //infoLabel->setText(tr("Invoked <b>File|Open</b>"));
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Document"), 
        "", tr("Bla bla bla (*.th)"));
    qDebug() << "open file " << fileName;

    if (fileName.length() == 0)
        return;

    mDoc.Load( fileName);
    
    //emit dataChanged();
    this->setSetup();     // apply mDoc.mSetup

    mOutput->mDataTab->setModel( &mDoc.rData() );
    mOutput->mGraphTab->reinitData( &mDoc.rData() );
}

void MainWindow::save()
{
    //infoLabel->setText(tr("Invoked <b>File|Save</b>"));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Document as"), 
        "", tr("Bla bla bla (*.th)"));
    qDebug() << "save file as" << fileName;

    if (fileName.length() == 0)
        return;

    mDoc.Save( fileName);
}

void MainWindow::select_export_tmpl()
{
    //QFileDialog dialog(this);
    //dialog.selectFile( mDoc.cTmplName() );
    //dialog.setNameFilter( tr("Latex template (*.ttex)") );
    //dialog.setViewMode(QFileDialog::Detail);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Select report template"), mDoc.cTmplName(), tr("Latex template (*.ttex)"));
    qDebug() << "Select report template" << fileName;

    if (fileName.length() == 0)
        return;

    mDoc.rTmplName() = fileName;
}

void MainWindow::export_rep()
{
    //infoLabel->setText(tr("Invoked <b>File|Save</b>"));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export report"), 
        "", tr("Text Report (*.txt);;Latex Report (*.tex)"));
    qDebug() << "export report file as" << fileName;

    if (fileName.length() == 0)
        return;

    mDoc.GenerateReport( fileName);
}


void MainWindow::settings()
{
    DlgSettings dialog( this);

    dialog.initValues( this->mSetup.cConfig() );
    int res = dialog.exec();
    if ( res == QDialog::Accepted )
    {
        dialog.fetchValues( this->mSetup.rConfig() );

        // TODO :: send new config and reset tool
        ThspMessage msg;

        //this->mSetup.packCfgMotor(msg);
        //mthSerial.request( ESerialRequest::TRANSMIT, msg );        

        this->mSetup.packCfgStg(msg);
        mthSerial.request( ESerialRequest::TRANSMIT, msg );        

        //this->mSetup.packCfgAdc(msg);
        //mthSerial.request( ESerialRequest::TRANSMIT, msg );        

        //this->mSetup.packCfgRpm(msg);
        //mthSerial.request( ESerialRequest::TRANSMIT, msg );        

        //this->mSetup.packCfgConvert(msg);
        //mthSerial.request( ESerialRequest::TRANSMIT, msg );

        ////QTimer::singleShot(200, this, &MainWindow::ack_settings);
        //// THSP_WRITE_CONFIG_TO_FLASH
    }
}

void MainWindow::about()
{
    QMessageBox msgBox;
    msgBox.setText( tr("The <b>ThrustApp</b> is used for managing the thrust measurement tool.\n") );
    msgBox.setInformativeText(mDoc.cHeader().strBoardInfo() + "\n"+ mDoc.cHeader().strSoftInfo() + "\n"+ mDoc.cHeader().strBuildInfo());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();

    //QMessageBox::about(this, tr("About Menu"),
    //        tr("The <b>ThrustApp</b> is used for managing the thrust measurement tool.\n")
    //    + mpDoc->strBoardInfo() + "\n"+ mpDoc->strSoftInfo() + "\n"+ mpDoc->strBuildInfo() );
    //msgBox.setInformativeText("Do you want to save your changes?");
}

void MainWindow::setSetup()
{
    const MeasurementSetup& setup = this->mDoc.rSetup();

    mControl->mFormSetup->editMotorName->setText( setup.cMotorName() );
    mControl->mFormSetup->spinMotorKV->setValue( setup.cMotorKV() );
    mControl->mFormSetup->spinMotorPolesCount->setValue( setup.cMotorPoles() );

    if ( setup.cEscTelemEnabled() ) 
        mControl->mFormSetup->checkBox_EscTelem->setChecked(true);

    if ( setup.cCurZeroEnabled() )
        mControl->mFormSetup->checkBox_CurrZero->setChecked(true);

    mControl->mFormSetup->editPropName->setText( setup.cPropName() );
    mControl->mFormSetup->spinBladeCount->setValue( setup.cPropBlades() );

    mControl->mFormSetup->editComment->setText( setup.cComment() );

    // TODO :: could be disconnected
    //if ( setup.cEscTelemEnabled() || setup.cRpmSource() == ERpmSource::EscTelem )
    //{
    //    mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_ESCTELEM_START) );        
    //}
    //else
    //{
    //    mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_ESCTELEM_STOP) );        
    //}

}

void MainWindow::applySetup()
{
    // update document setup
    MeasurementSetup& setup = this->mDoc.rSetup();

    setup.rMotorName() = mControl->mFormSetup->editMotorName->text();
    setup.rMotorKV() = mControl->mFormSetup->spinMotorKV->value();
    setup.rMotorPoles() = mControl->mFormSetup->spinMotorPolesCount->value();

    setup.rEscTelemEnabled() = mControl->mFormSetup->checkBox_EscTelem->isChecked();
    setup.rCurZeroEnabled() = mControl->mFormSetup->checkBox_CurrZero->isChecked();

    setup.rPropName() = mControl->mFormSetup->editPropName->text();
    setup.rPropBlades() = mControl->mFormSetup->spinBladeCount->value();

    setup.rComment() = mControl->mFormSetup->editComment->text();

    setup.rRpmSource() = ERpmSource::None;
    if ( mControl->mFormSetup->radioButton_Sensor->isChecked() )
        setup.rRpmSource() = ERpmSource::IrSensor;
    else if ( mControl->mFormSetup->radioButton_Telem->isChecked() )
        setup.rRpmSource() = ERpmSource::EscTelem;

    // TODO :: apply new setup to measurement
    
    if ( setup.cEscTelemEnabled() || setup.cRpmSource() == ERpmSource::EscTelem )
    {
        mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_ESCTELEM_START) );        
    }
    else
    {
        mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_ESCTELEM_STOP) );        
    }

    //if ( mControl->mFormSetup->radioButton_Sensor->isChecked() )
    //{
    //    mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_RPM_START) );        
    //}
    //else
    //{
    //    mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_RPM_STOP) );        
    //}

    mDoc.initConverter( mSetup);
}


void MainWindow::start_connection()
{
    if ( mSerialStat == ESerialStatus::Closed )
    {
        QString portName    = mControl->mFormConnect->comboPort->currentText();
        //QString rate        = mControl->mFormConnect->comboRate->currentText();
        int baudRate        = mControl->mFormConnect->comboRate->currentData().toInt();

        qDebug() << "MainWindow::start_connection() -- port = " << portName.toUtf8() <<  ", baudrate = " << baudRate;

        mthSerial.startConnection( portName, baudRate, 300 );
        mSerialStat = ESerialStatus::Waiting;
        labConnStatus->setText( tr("Waiting") );
    }
    else
    {
        qDebug() << "MainWindow::start_connection() -- serial is not closed";
    }
}

void MainWindow::stop_connection()
{
    if ( mSerialStat == ESerialStatus::Open )
    {
        qDebug() << "MainWindow::stop_connection() -- closing connection";
        mthSerial.closeConnection();
    }
}

void	MainWindow::connected( bool flag)
{
    if ( flag)
    {
		mControl->mFormConnect->btnConnect->setDisabled( true);

        // gather info
        qDebug() << "MainWindow::connected() -- gathering info";

        mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_API_VERSION) );        
        QTimer::singleShot(200, this, &MainWindow::check_connection);
        
        qDebug() << "MainWindow::connected() -- serial is OPEN";

        this->startHBeat();
    }
    else
    {
        after_disconnect();

        qDebug() << "MainWindow::connected() -- serial is CLOSED";
    }
}

void MainWindow::check_connection()
{
    if ( mSerialStat == ESerialStatus::Open )
    {
        //mSerialStat = ESerialStatus::Open;
		mControl->mFormConnect->btnDisconnect->setEnabled( true);
        discAct->setEnabled(true);
        labConnStatus->setText( tr("Connected: ") + mControl->mFormConnect->comboPort->currentText() );

        QString portName    = mControl->mFormConnect->comboPort->currentText();
        int baudRate        = mControl->mFormConnect->comboRate->currentData().toInt();
        this->mOutput->mTermTab->setPortInfo( portName, baudRate);


        // enable widgets
        mControl->enableForms();
        armAct->setEnabled(true);
    }
    else
    {
        arm( false);
        // TODO :: wait for ack_arm

        mthSerial.closeConnection(); // CHECKME :: 
        after_disconnect();
    }
}


//void MainWindow::disconnect()
//{
//    arm( false);
//    // TODO :: wait for ack_arm
//
//    close_connection();
//}

void MainWindow::after_disconnect()
{
    mSerialStat = ESerialStatus::Closed;

	mControl->mFormConnect->btnConnect->setEnabled( true);
	mControl->mFormConnect->btnDisconnect->setDisabled( true);
    discAct->setDisabled(true);       
    labConnStatus->setText( tr("Disconnected") );

    armAct->setText(tr("Arm"));
    armAct->setStatusTip(tr("Arm motor"));
    armAct->setChecked(false);
    labMotorStatus->setText( tr("Disarmed"));

    // disable widgets
    mControl->disableForms();
    armAct->setDisabled(true);

    // shutdown hbeat timer
    stopHBeat();
}


void MainWindow::startHBeat()
{
    mHBeatTimer = new QTimer(this);
    connect(mHBeatTimer, &QTimer::timeout, this, &MainWindow::requestHBeat);
    mHBeatTimer->start( 1000);
}


void MainWindow::stopHBeat()
{
    // shutdown hbeat timer
    if (mHBeatTimer)
        delete mHBeatTimer;
    mHBeatTimer = nullptr;
    //mbHBeat = false;
}


void MainWindow::requestHBeat()
{
    mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_SEND_HARTBEAT) );        

    qDebug() << "MainWindow::requestHBeat";
    //QTimer::singleShot(200, this, &MainWindow::check_connection);
}


void MainWindow::arm( bool b)
{
    if ( mSerialStat != ESerialStatus::Open )
    {
        armAct->setChecked(false);
        return;
    }

    mbackArm = false;

    if ( b )
    {
        armAct->setText(tr("Disarm"));
        armAct->setStatusTip(tr("Disarm motor"));
        labMotorStatus->setText( tr("ARMED"));
        mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_MOTOR_ARM) );
    }
    else
    {
        armAct->setText(tr("Arm"));
        armAct->setStatusTip(tr("Arm motor"));
        labMotorStatus->setText( tr("Disarmed"));
        mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_MOTOR_DISARM) );
    }

    //QTimer::singleShot(200, this, &MainWindow::ack_arm(true) );
    QTimer::singleShot(200, this, [this, b]() { this->ack_arm(b); } );

}

void MainWindow::ack_arm( bool b)   // TODO :: check this !!!
{
    if (b)
    {
        if ( ! mbackArm )   // arming failed !
        {
            armAct->setText(tr("Arm"));
            armAct->setStatusTip(tr("Arm motor"));
            armAct->setChecked(false);
            labMotorStatus->setText( tr("Disarmed"));
        }
    }
    else
    {
        if ( ! mbackArm )   // disarming failed !
        {
            armAct->setText(tr("Disarm"));
            armAct->setStatusTip(tr("Disarm motor"));
            armAct->setChecked(true);
            labMotorStatus->setText( tr("ARMED"));
        }
    }

    //mbackArm = false;
}


void MainWindow::start_calib( int n)
{
    ASSERT(0);

    //mcountCalib = n;
    send_cmnd( THSP_MEASURE_START );
    send_cmnd( THSP_DATASTREAM_START );
}


void MainWindow::send()
{
    if ( mSerialStat == ESerialStatus::Open )
    {
        int cmnd = mOutput->mDebugTab->lineEdit_Command->text().toInt();
        int32_t data = (int32_t)mOutput->mDebugTab->lineEdit_Data->text().toInt();

        //qDebug() << "port = " << portName.toUtf8().constData() <<  " baudrate = " << baudRate << "\n";

        ThspMessage msg( THSP_PACKET_COMMAND, cmnd);
        SBuf sbuf( msg.data);
        sbuf.WriteU32( (uint32_t) data );
        msg.dataSize = (uint16_t)sbuf.Length();

        qDebug() << "MainWindow::send() -- msg" <<  msg.command << "- sent";
        mthSerial.request( ESerialRequest::TRANSMIT, msg );
    }
    else
    {
        qDebug() << "MainWindow::send() -- serial is not open";
    }

}


void MainWindow::send_cmnd( uint8_t code, uint16_t data )
{
    if ( mSerialStat == ESerialStatus::Open )
    {
        ThspMessage msg( THSP_PACKET_COMMAND, code);

        if ( data != 0 )
        {
            SBuf sbuf( msg.data);
            sbuf.WriteU16( (uint16_t) data );
            msg.dataSize = (uint16_t) sbuf.Length();
        }

        qDebug() << "MainWindow::send(...) -- msg" <<  msg.command << ":" <<  msg.data << "- sent";
        mthSerial.request( ESerialRequest::TRANSMIT, msg );
    }
    else
    {
        qDebug() << "MainWindow::send(...) -- serial is not open";
    }
}

void MainWindow::send_cmnd(uint8_t code, uint8_t* pdata, uint8_t length)
{
    if ( mSerialStat == ESerialStatus::Open )
    {
        ThspMessage msg( THSP_PACKET_COMMAND, code);

        if ( pdata != nullptr && length > 0 )
        {
            SBuf sbuf( msg.data);
            sbuf.WriteData( pdata, length );
            msg.dataSize = (uint16_t) sbuf.Length();
        }

        qDebug() << "MainWindow::send(...) -- msg" <<  msg.command << ": data size = " <<  length << "- sent";
        mthSerial.request( ESerialRequest::TRANSMIT, msg );
    }
    else
    {
        qDebug() << "MainWindow::send(...) -- serial is not open";
    }
}


void MainWindow::receive(bool b)
{
    QQueue<ThspMessage> queMsg;
    mthSerial.getReply( queMsg);

    bool bemit = false;

    //for ( auto& msg :tabmsg)

    while ( queMsg.size() > 0 )
    {
        ThspMessage msg = queMsg.dequeue();

        if ( msg.command == THSP_SEND_DATA )
        {
            SBuf	sbin( msg.data);
            uint32_t msg_id = sbin.ReadU32();

           // qDebug() << "MainWindow::receive() -- msg" <<  msg.command << ":" << msg_id << "- received";
        }
        else
        {
            qDebug() << "MainWindow::receive() -- msg" <<  msg.command << "- received";
        }

        QString strCmnd = QString::number( msg.command);
        QString strData;
        QString strDataHex;
        QString text;

        for (int i=0; i<msg.dataSize; ++i)
        {
            strData += QString::number( msg.data[i] ) + " : ";
            strDataHex += "0x" + QString::number( msg.data[i], 16 ) + " : ";
            //strDataHex += QByteArray::fromHex( QString::number( msg.data[i]).toUtf8() )  + " : ";
            text += QString( (char)msg.data[i] );
        }
        //hexadecimal = QByteArray::fromHex(QString::number(decimal));


        QString s = "Msg code: " + strCmnd + "\n- " + strData + "\n\n- " + strDataHex + "\n\n- \"" + text + "\"\n\n\n";

        mOutput->mDebugTab->txtView->moveCursor( QTextCursor::End);
        mOutput->mDebugTab->txtView->insertPlainText( s);


        SBuf sbin( msg.data);

        switch ( msg.command )
        {
        case THSP_API_VERSION:
        {
    	    SBuf	sbin( msg.data);

    	    uint8_t prot_ver = sbin.ReadU8();
    	    uint8_t api_major = sbin.ReadU8();
    	    uint8_t api_minor = sbin.ReadU8();
            if ( prot_ver == THSP_API_VERSION && api_major == API_VERSION_MAJOR && api_minor == API_VERSION_MINOR)
            {
                mSerialStat = ESerialStatus::Open;

                mDoc.rHeader().setApiVer( prot_ver, api_major, api_minor);

                qDebug() << "Connection success :  thsp_device found";

                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_BOARD_INFO) );
                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_SOFT_INFO) );
                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_BUILD_INFO) );

                // TODO :: get board config + channels info
                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_GET_CH_COUNT) );

                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_GET_CONFIG_ADC) );
                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_GET_CONFIG_STGAUGE) );
                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_GET_CONFIG_MOTOR) );
                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_GET_CONFIG_RPM) );
                mthSerial.request( ESerialRequest::TRANSMIT, ThspMessage( THSP_PACKET_COMMAND, THSP_GET_CONFIG_CONVERT) );
            }
            else
            {
                qDebug() << "Unrecognized device : " << prot_ver << api_major << api_minor;           
                stop_connection();
            }

            break;
        }

        case THSP_SOFT_INFO:
            mDoc.rHeader().setSoftInfo( QByteArray( (char*)msg.data, msg.dataSize) );
            break;

        case THSP_BOARD_INFO:
            mDoc.rHeader().setBoardInfo( QByteArray( (char*)msg.data, msg.dataSize) );
            break;

        case THSP_BUILD_INFO:
            mDoc.rHeader().setBuildInfo( QByteArray( (char*)msg.data, msg.dataSize) );
            break;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case THSP_GET_CH_COUNT:
            if ( ! mSetup.initChannels( msg) )
            {
                // Error - incompatible channel size between app & tool
                QMessageBox::critical( this,	tr("Error"), tr("Incompatible channel sizes between app and tool") );
                //errorTool( "Incompatible channel sizes between app and tool");
            }
            break;

        case THSP_GET_CONFIG_ADC:
            mSetup.initCfgAdc( msg);
            break;

        case THSP_GET_CONFIG_STGAUGE:
            mSetup.initCfgStg(msg);
            break;

        case THSP_GET_CONFIG_MOTOR:
            mSetup.initCfgMotor( msg);
            break;

        case THSP_GET_CONFIG_RPM:
            mSetup.initCfgRpm( msg);
            break;

        case THSP_GET_CONFIG_CONVERT:
            mSetup.initCfgConvert( msg);

            mDoc.initConverter( mSetup);
            break;

        case THSP_GET_DATA_ZERO:
            break;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case THSP_SHELL_START:
            break;

        case THSP_MOTOR_ARM:
            mbackArm = true;
            break;

        case THSP_MOTOR_DISARM:
            mbackArm = true;
            break;

        case THSP_SEND_HARTBEAT:
            {
                //mbHBeat = true;

                SBuf buf( msg.data);
                uint16_t ibat = (float)buf.ReadU16();
                uint16_t iref = (float)buf.ReadU16();

                double volt = mDoc.cConverter().convertVBat( ibat, iref);
                QString sbat = QString::number(volt, 'f', 2) + "V";
                qDebug() << "hbeat vbat = " << sbat;
                labVBatCell->setText( sbat );

                break;
            }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        case THSP_SEND_DATA:
            {
                size_t curid = mDoc.cData().size();
                mDoc.appendData( msg );
                
                int nskip = mSetup.cConfig().groupStrain.sampleRate == HX711_RATE_80SPS ? 40 : 5;
                if ( mDoc.cData().size() % nskip  == 0 )
                {
                    mOutput->mGraphTab->updateData( &mDoc.cData(), 5 );
                }

                bemit = true;

                break;
            }
        }
    }

    if ( bemit)
    {

        emit dataReceived();
    }

}


//void MainWindow::initCoverter()
//{
//    mDoc.initConverter( mSetup);
//}


//void MainWindow::errorTool( const QString & s)
//{
//    qDebug() << "MainWindow::errorTool() -- ERROR: '" << s << "'";
//    QMessageBox::critical( this,	tr("Error"), s );
//
//    disconnect();
//}

 
// btype; true -> read, false -> send
void MainWindow::timeoutHandler( bool btype, int ccode )
{
    if ( ccode == THSP_SEND_HARTBEAT )
    {
        //mbHBeat = false;

        // disconnect
        mthSerial.closeConnection();
        this->after_disconnect();

        QMessageBox::warning( this,	tr("Timeout"), "Connection lost" );
        return;
    }


    QString s;
    if ( btype )
    {
        s = "Timeout on receiving - msg code " + QString::number(ccode);

        qDebug() << "MainWindow::timeoutHandler() -- TIMEOUT RECEIVE: '" << ccode << "'";
        
        switch ( ccode )
        {
        case THSP_MOTOR_ARM:
            qDebug() << " -- timeout THSP_MOTOR_ARM :";
            break;

        default:
            qDebug() << " -- timeout receiving cmnd:" << ccode;
        };
    }
    else
    {
        s = "Timeout on sending - msg code " + QString::number(ccode);

        qDebug() << "MainWindow::timeoutHandler() -- TIMEOUT SEND: '" << ccode << "'";
    }

    QMessageBox::warning( this,	tr("Timeout"), s );
}


void MainWindow::errorHandler(const QString & s)
{
    qDebug() << "MainWindow::errorHandler() -- ERROR: '" << s << "'";
    QMessageBox::critical( this,	tr("Error"), s );
}
