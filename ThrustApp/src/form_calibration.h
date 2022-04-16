#ifndef  __FORM_CALIBRATION_H__
#define __FORM_CALIBRATION_H__

#include "ui_form_calibration.h"
//#include "widget_control.h"
#include "stat_value.hpp"


class ControlWidget;
class MainWindow;
struct ThspMessage;

enum class ECalibType
{
    Thrust,
    Torque
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormCalibration
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormCalibration : public QWidget, public Ui::FormCalibration
{
    Q_OBJECT

public:
    explicit FormCalibration(QWidget *parent = nullptr);

    void linkMainWindow( MainWindow* pwin)  { mpWin = pwin; }


    void onRadioThrust( bool);
    void onRadioTorque( bool);

    void onExecute();
    void updateExecute( int32_t thrust, int32_t torque);
    void finishExecute();

    void onAddPoint();
    void onCalc();

private slots:
    void    onContextMenu( const QPoint& );
    void    onDeleteRow();

private:
    void    updateTime( int n);

private:
    MainWindow* mpWin;

    ECalibType  mType;

    double          mLoad;
    StatValueS24    mstatThrust;
    StatValueS24    mstatTorque;

    std::vector< std::pair< double, double > > mtabPnts;

};


#endif // __FORM_CALIBRATION_H__
