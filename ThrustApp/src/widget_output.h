#ifndef __WIDGET_OUTPUT_H__
#define __WIDGET_OUTPUT_H__

#include <QTabWidget>

#include "view_graph.h"
#include "view_terminal.h"
#include "view_data.h"

#include "ui_form_debugmsg.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormDebugMsg
////////////////////////////////////////////////////////////////////////////////////////////////////
class FormDebugMsg : public QWidget, public Ui::FormDebugMsg
{
    Q_OBJECT

public:
    explicit FormDebugMsg(QWidget *parent );

private:
};


class Document;
class MainWindow;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class OutputWidget
////////////////////////////////////////////////////////////////////////////////////////////////////
class TabWidget : public QWidget
{
    Q_OBJECT

    friend class MainWindow;

public:
    TabWidget( Document* pdoc, MainWindow *parent );

public slots:
    //void    onDataChenged();

private:
    QTabWidget*     mTab;

    GraphView*      mGraphTab;
    DataView*       mDataTab;
    TerminalView*   mTermTab;
    FormDebugMsg*   mDebugTab;
};


#endif // __WIDGET_OUTPUT_H__
