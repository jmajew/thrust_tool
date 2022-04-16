#ifndef __DLG_SETTINGS_H__
#define __DLG_SETTINGS_H__

#include <QDialog>
#include <QTabWidget>
#include <QPushButton>
#include <QDebug>

#include "appsetup.h"

#include "ui_form_cfg_tool.h"
#include "ui_form_cfg_convert.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormCfgTool
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormCfgTool : public QWidget, public Ui::FormCfgTool
{
    Q_OBJECT

public:
    explicit FormCfgTool(QWidget *parent = nullptr);

    void    initValues( const Config& cfg);
    void    fetchValues( Config& cfg);

private:
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormCfgConvert
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormCfgConvert : public QWidget, public Ui::FormCfgConvert
{
    Q_OBJECT

public:
    explicit FormCfgConvert(QWidget *parent = nullptr);

    void    initValues( const Config& cfg);
    void    fetchValues( Config& cfg);

private:
};


//////////////////////////////////////////////////////////////////////
// class DlgSettings
//////////////////////////////////////////////////////////////////////
class DlgSettings : public QDialog
{
	Q_OBJECT

public:
//	Ui::DlgLineStyle ui;

	DlgSettings( QWidget* parent);

    void    initValues( const Config& cfg);
    void    fetchValues( Config& cfg);

protected:

public slots:

private:
    QTabWidget*     mTab;

    FormCfgTool*    mCfgToolTab;
    FormCfgConvert* mCfgConvertTab;

    QPushButton*    buttonOk;
    QPushButton*    buttonCancel;
}; 


#endif // __DLG_SETTINGS_H__
