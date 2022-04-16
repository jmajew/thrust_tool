#ifndef __VIEW_GRPAH_H__
#define __VIEW_GRPAH_H__

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QToolBar>

#include <QRectF>
#include <QVector2D>

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>

#include "cf_gvector.h"


//QT_BEGIN_NAMESPACE
//class QPixmap;
//class QGraphicsSceneContextMenuEvent;
//class QMenu;
//class QPolygonF;
//QT_END_NAMESPACE


class Document;

class Layer
{
public:
    Layer() : mvOrigin({0.0}), mvScale({1.0}), mvArea( {500,400} )  
    {
        double marg = 20;
        rect.setRect( marg, marg, mvArea.rX() - 2*marg, mvArea.rY() - 2*marg);
    }

    void    SetSize( qreal sx, qreal sy)    
    { 
        mvArea.rX() = sx;  
        mvArea.rY() = sy;
    }

    void    SetTargetRect( int w, int h )
    {
        mvScale.rX() = (qreal)w / mvArea.cX();
        mvScale.rY() = (qreal)h / mvArea.cY();
    }

    QPointF toViewport( CFGeom::Vect2D v) const
    {
        v -= mvOrigin;
        return QPointF( v.cX() * mvScale.cX(), v.cY() * mvScale.cY() );
    }

    QPointF toViewport( QPointF pnt) const
    {
        pnt -= QPointF( mvOrigin.cX(), mvOrigin.cY() );
        return QPointF( pnt.rx() * mvScale.cX(), pnt.ry() * mvScale.cY() );
    }

    void    Paint( QRectF viewrec, QPainter* painter);

public:
    CFGeom::Vect2D  mvOrigin;
    CFGeom::Vect2D  mvScale;

    CFGeom::Vect2D  mvArea;

    QRectF          rect;

    QVector<CFGeom::Vect2D> tabPoint;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//// class PlotScene
//////////////////////////////////////////////////////////////////////////////////////////////////////
//class PlotScene : public QGraphicsScene
//{
//public:
//    explicit PlotScene( QObject *parent );
//
//};




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
class RenderArea;

////////////////////////////////////////////////////////////////////////////////////////////////////
// class Channel
////////////////////////////////////////////////////////////////////////////////////////////////////
class Channel //: public QObject
{
    //Q_OBJECT

public:
    Channel( int id, const std::string& name/*, QObject *parent = nullptr*/) 
        : /*QObject(parent), */mId(id), mName(name), mbEnabled(true)    {}

    void    SetEnabled( bool b)     { mbEnabled = b; }
    void    AddValue( double v);

    void    InitDummy( int n = 100);
    void    Paint( RenderArea* prend);

public:
    int         mId;
    std::string mName;

    bool    mbEnabled;
    double  mValMin;
    double  mValMax;
    std::vector<double>     mtabVals;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// class ViewData
////////////////////////////////////////////////////////////////////////////////////////////////////
class ViewData
{
public:
    std::vector<Channel>  mtabCh;
};


////////////////////////////////////////////////////////////////////////////////////////////////////
// class RenderArea
////////////////////////////////////////////////////////////////////////////////////////////////////
class RenderArea : public QWidget
{
    Q_OBJECT

public:
    explicit RenderArea( ViewData* pdat, QWidget *parent );

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

public slots:
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void mousePressEvent( QMouseEvent *event) override;
	void mouseReleaseEvent ( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event) override;
	void wheelEvent( QWheelEvent *event) override;

private:
    QPen pen;
    QBrush brush;
    bool antialiased;
    bool transformed;
    QPixmap pixmap;

    //Layer   layer;
    ViewData*   mpData;
};




////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphView
////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphView : public QWidget
{
	Q_OBJECT

public:
	explicit GraphView( QWidget* parent );


    void    updateData( Document* pdoc);
	
private:
    QAction*        mptestAct;
	QToolBar*		mpToolBar;
    RenderArea*     mpRenderWidget;

    ViewData        mData;
};


#endif // __VIEW_GRPAH_H__
