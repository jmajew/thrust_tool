#include <QDebug>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QScrollBar>

#include <QGraphicsLineItem>

#include "document.h"
#include "view_graph.h"
#include "dlg_graphstyle.h"
#include "dlg_graphprobe.h"





////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphView
////////////////////////////////////////////////////////////////////////////////////////////////////

GraphView::GraphView( const Document* ptr, QWidget* parent) 
	: QWidget(parent)
{
	mpToolBar = new QToolBar(this);
	//mpToolBar->setFixedHeight(22);

    mpresetAct = new QAction(tr("&Reset"), this);
    //mptestAct->setShortcuts(QKeySequence::New);
    mpresetAct->setStatusTip(tr("Reset measurement data"));
    //connect(mptestAct, &QAction::triggered, this, &MainWindow::newFile);
    mpToolBar->addAction(mpresetAct);

    QAction* pactZAll = new QAction( tr("Zoom All"));
    mpToolBar->addAction(pactZAll);

    //QAction* pactStyle = new QAction( tr("Style"));
    //mpToolBar->addAction(pactStyle);

    mpprobeAct = new QAction( tr("Probe"));
    mpprobeAct->setCheckable(true);
    mpprobeAct->setChecked(false);
    mpToolBar->addAction(mpprobeAct);

    mpstyleAct = new QAction( tr("TStyle"));
    mpstyleAct->setCheckable(true);
    mpstyleAct->setChecked(false);
    mpToolBar->addAction(mpstyleAct);

    //--------------------------------------------------------------------------
    // grapher
    mpGraph = new GraphWidget( ptr, this);


    //--------------------------------------------------------------------------
    // layout
	QVBoxLayout* pVBox = new QVBoxLayout(this);

    pVBox->addWidget( mpToolBar);
    pVBox->addWidget( mpGraph);

    pVBox->setContentsMargins(1,1,1,1);
    pVBox->setSpacing(1);

	setLayout(pVBox);

    connect( mpresetAct, &QAction::triggered, this, &GraphView::resetData );
    connect( pactZAll, &QAction::triggered, this->mpGraph, &GraphWidget::zoomAll );

//    connect( pactStyle, &QAction::triggered, this->mpGraph, &GraphWidget::setStyle );

    connect( mpstyleAct, &QAction::toggled, this->mpGraph, &GraphWidget::showStyle );
    connect( mpGraph->mpDlgStyle, &DlgGraphStyle::sigHidden, this, &GraphView::onStyleHide );


    connect( mpprobeAct, &QAction::toggled, this->mpGraph, &GraphWidget::showProbe );
    connect( mpGraph->mpDlgProbe, &DlgGraphProbe::sigHidden, this, &GraphView::onProbeHide );
}

void GraphView::onProbeHide()
{
    this->mpprobeAct->setChecked(false);
}

void GraphView::onStyleHide()
{
    this->mpstyleAct->setChecked(false);
}


void GraphView::updateData( const MeasurementData* pdat, int count)
{
    if ( pdat != nullptr)
    {
        qDebug() << "GraphView::updateData - called";

        this->mpGraph->updateData( pdat, count);
    }
}

void GraphView::resetData()
{
    this->mpGraph->resetData();

    //emit resetMeasurement();
}



