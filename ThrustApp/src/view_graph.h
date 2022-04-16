#ifndef __VIEW_GRAPH_H__
#define __VIEW_GRAPH_H__

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QToolBar>

#include <QRectF>
#include <QVector2D>

#include "widget_graph.h"


//QT_BEGIN_NAMESPACE
//class QPixmap;
//class QGraphicsSceneContextMenuEvent;
//class QMenu;
//class QPolygonF;
//QT_END_NAMESPACE

class GraphView;
class Document;
class MeasurementData;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphView
////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphView : public QWidget 
{
	Q_OBJECT

public:
	explicit GraphView( const Document* ptr, QWidget* parent );

    //void togglePointerMode()
    //{
    //    mpView->setDragMode( QGraphicsView::ScrollHandDrag);
    //    mpView->setInteractive(false);
    //}

    void    updateData( const MeasurementData* pdat, int count);

    void    reinitData( MeasurementData* pdat)    { mpGraph->reinitData(pdat); }
	
public slots:
    void    resetData();
    void    onProbeHide();
    void    onStyleHide();

signals:
    //void    resetMeasurement();

private:
    QAction*        mpresetAct;
    QAction*        mpprobeAct;
    QAction*        mpstyleAct;

	QToolBar*		mpToolBar;


    GraphWidget*    mpGraph;
};


#endif // __VIEW_GRAPH_H__
