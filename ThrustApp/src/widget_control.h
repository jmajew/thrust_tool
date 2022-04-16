#ifndef __WIDGET_CONTROL_H__
#define __WIDGET_CONTROL_H__

#include <array>

#include <QToolBox>
#include <QAbstractTableModel>

#include "form_calibration.h"

#include "ui_form_connect.h"
#include "ui_form_measuresettings.h"
#include "ui_form_measurement.h"
#include "ui_form_automeasurement.h"
#include "ui_form_process.h"

#include "form_automeasurement.h"



class MainWindow;
class MeasurementSetup;
class MeasurementData;


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormConnection
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormConnection : public QWidget, public Ui::FormConnection
{
    Q_OBJECT

public:
    explicit FormConnection(QWidget *parent = nullptr);

private:
    void    FillPortsInfo();
    void    ShowPortInfo( int idx);

};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormSetup
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormSetup : public QWidget, public Ui::FormMeasureSettings
{
    Q_OBJECT

public:
    explicit FormSetup(QWidget *parent = nullptr);

    void    InitValues( const MeasurementSetup& setup);

private:
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormMeasurement
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormMeasurement : public QWidget, public Ui::FormMeasurement
{
    Q_OBJECT

public:
    explicit FormMeasurement(QWidget *parent = nullptr);

};




////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormProcess
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormProcess : public QWidget, public Ui::FormProcess
{
    Q_OBJECT

public:
    explicit FormProcess(QWidget *parent = nullptr);

public slots:

private:
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class ControlWidget
////////////////////////////////////////////////////////////////////////////////////////////////////
class ControlWidget : public QWidget
{
    Q_OBJECT

    friend class MainWindow;

    void enableForms();
    void disableForms();

public:
    ControlWidget(QWidget *parent = nullptr);

    void linkMainWindow( MainWindow* pwin);

private:
    QToolBox*           mBox;

    FormConnection*         mFormConnect;
    //FormCalibration*        mFormCalibrate;
    FormSetup*              mFormSetup;
    FormMeasurement*        mFormMeasurement;
    FormAutoMeasurement*    mFormAutoMeasure;
    FormProcess*            mFormProcess;
};


#endif // __WIDGET_CONTROL_H__
