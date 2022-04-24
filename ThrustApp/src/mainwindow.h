#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "widget_control.h"
#include "widget_output.h"
#include "appsetup.h"
#include "document.h"
#include "thread_serial.h"
#include "controller.h"
#include "analyzer.h"


enum class ESerialStatus
{
    Closed,
    Open,
    Waiting
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class MainWindow
////////////////////////////////////////////////////////////////////////////////////////////////////
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent);
    ~MainWindow();

signals:
	void dataChanged();
    void dataReceived();


public:
    bool isSerialActive() const     { return ( mSerialStat == ESerialStatus::Open ); }
    bool isArmed() const            { return mbackArm; }        //armAct->checked()

    void startMotor( int level)     { level = level*10 + 1000; this->send_cmnd( THSP_MOTOR_SET_COMMAND, level); }
    void stopMotor()                { this->send_cmnd( THSP_MOTOR_STOP); }

    void startMeasurement()         { this->send_cmnd( THSP_DATASTREAM_START); }
    void stopMeasurement()          { this->send_cmnd( THSP_DATASTREAM_STOP ); }


    void send();
    void send_cmnd( uint8_t code, uint16_t data = 0 );
    void send_cmnd( uint8_t code, uint8_t* pdata, uint8_t length );
    void receive( bool b);

    void applySetup();
    void setSetup();
    void start_calib( int n);

    const Document&     cDoc() const        { return mDoc; }

private:
    void onDeviceReset();
	void onProcess( const MeasurementData* data);

    void newFile();
    void open();
    void save();
    void select_export_tmpl();
    void export_rep();
    void settings();
    void about();

    void start_connection();
    void stop_connection();
    void connected( bool flag);
    void check_connection();
    void after_disconnect();
    void disconnect();

    void start_terminal();
    void stop_terminal();

    void reset_tool();

    void requestHBeat();
    void startHBeat();
    void stopHBeat();

    
    void arm( bool b);
    void ack_arm( bool b);

    //void errorTool( const QString & s);
    void errorHandler( const QString & s);
    void timeoutHandler( bool btype, int ccode ); // btype; true -> read, false -> send

private:
    //void initCoverter();

private:
    ControlWidget*  mControl;
    TabWidget*      mOutput;

    QTimer*         mHBeatTimer;
    bool            mbHBeat;

    QAction*        discAct;
    QAction*        armAct;
    bool            mbackArm;   // acknowledge

    QLabel*     labConnStatus;
    QLabel*     labMotorStatus;
    QLabel*     labVBatCell;

private:
    SerialThread    mthSerial;
    ESerialStatus   mSerialStat;

    Controller      mAutoControl;
    Analyzer        mAnalyzer;      // TODO :: make it local where it is needed

    ToolSetup       mSetup;
    Document        mDoc;

};

#endif // MAINWINDOW_H
