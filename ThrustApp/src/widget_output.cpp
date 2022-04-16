#include <QLayout>
#include <QPushButton>
#include <QDebug>

#include "widget_output.h"
#include "document.h"
#include "mainwindow.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
// class FormDebugMsg
////////////////////////////////////////////////////////////////////////////////////////////////////
FormDebugMsg::FormDebugMsg(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// class OutputWidget
////////////////////////////////////////////////////////////////////////////////////////////////////

TabWidget::TabWidget( Document* pdoc, MainWindow *parent) : QWidget(parent)   //, mpDoc(nullptr)
{
    mTab = new QTabWidget( this);

    //QWidget* widget = new QWidget(mTab);
    //Ui::FormConnection ui;
    //ui.setupUi(widget);
    //mTab->addTab(widget, tr("Graph View"));

    parentWidget();

    mGraphTab = new GraphView( pdoc, this );
    mDataTab = new DataView( pdoc, this );
    mTermTab = new TerminalView( this );
    mDebugTab = new FormDebugMsg( this );

    mTab->addTab( mGraphTab, tr("Graph View"));
    mTab->addTab( mDataTab, tr("Data View"));
    mTab->addTab( mTermTab, tr("Terminal"));
    mTab->addTab( mDebugTab, tr("Debug"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mTab);
    setLayout(mainLayout);
}

