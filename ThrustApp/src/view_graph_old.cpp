#include <random>

#include <QDebug>
#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QFont>

#include <QGraphicsLineItem>

#include "document.h"
#include "view_graph.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//// class PlotScene
//////////////////////////////////////////////////////////////////////////////////////////////////////
//PlotScene::PlotScene( QObject *parent ) : QGraphicsScene(parent)
//{
//    QGraphicsLineItem *pline = this->addLine( 0,0,100,100, QPen(Qt::red, 2) );
//    QGraphicsRectItem *prec = this->addRect( 10, 10, 1490, 990, QPen( Qt::black, 3) );
//    this->addText( "Hello !!!", QFont( "SansSerif", 14, QFont::Bold) );
//    pline->setLine( 10,10, 1490, 990);
//
//}

void Layer::Paint( QRectF viewrec, QPainter* painter)
{
    SetTargetRect( viewrec.width(), viewrec.height() );

    painter->setPen( QPen(Qt::red, 5) );
    painter->drawRect( QRectF( toViewport(rect.topLeft()), toViewport(rect.bottomRight()) ) );
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


////////////////////////////////////////////////////////////////////////////////////////////////////
// class Channel
////////////////////////////////////////////////////////////////////////////////////////////////////

void Channel::AddValue( double v)
{
    if ( mtabVals.size() == 0 )
        mValMax = mValMin = v;

    if (mValMax < v)
        mValMax = v;

    if (mValMin > v)
        mValMin = v;

    mtabVals.push_back(v);
}


void Channel::InitDummy( int n )
{
    std::random_device dev;
    std::mt19937 rng(dev());
	std::uniform_int_distribution< int64_t > dist(-16777216, 16777216);

    for ( int i = 0; i<5; ++i) 
        AddValue( 10000000 );

    for ( int i = 0; i<n; ++i) 
        AddValue( dist(rng) );
}


void Channel::Paint( RenderArea* prend)
{
    QPolygon symb;
    symb << QPoint(-2, -2) << QPoint(2, -2) << QPoint(2, 2) << QPoint(-2, 2);

    const int xmarg = 5;
    const int ymarg = 5;

    QRect recPlot( QPoint(xmarg, ymarg), QPoint(prend->width()-xmarg, prend->height()-ymarg) ); 
    QPoint vorg = recPlot.topLeft();
    int w = recPlot.width();
    int h = recPlot.height();

    double scale_y = (double)h / (mValMax - mValMin);
    double scale_x = (double)w / (double)mtabVals.size();

    QPainter painter(prend);

    //QPen pen(Qt::red, 1);
    //painter.setPen(pen);

    painter.setPen(QPen(Qt::green, 2));
    for ( size_t i=1; i<mtabVals.size(); ++i )
    {
        QPoint pos0( (int)(scale_x*(i-1)), h - (int)(scale_y*(mtabVals[i-1] - mValMin)) );
        pos0 += vorg;
        QPoint pos1( (int)(scale_x*i), h - (int)(scale_y*(mtabVals[i] - mValMin)) );
        pos1 += vorg;

        painter.drawLine( pos0, pos1);
    }

    painter.setPen(QPen(Qt::red, 1));
    for ( size_t i=0; i<mtabVals.size(); ++i )
    {
        QPoint pos( (int)(scale_x*i), h - (int)(scale_y*(mtabVals[i] - mValMin)) );
        pos += vorg;

        QPolygon poly(symb);
        poly.translate(pos);

        painter.drawPolygon(poly);
    }

}



////////////////////////////////////////////////////////////////////////////////////////////////////
// class RenderArea
////////////////////////////////////////////////////////////////////////////////////////////////////


RenderArea::RenderArea( ViewData* pdat, QWidget *parent)
    : QWidget(parent), mpData(pdat)
{
    antialiased = false;
    transformed = false;
    pixmap.load(":/images/qt-logo.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    this->setPen( QPen(Qt::blue, 3) );

    //if (style == Qt::LinearGradientPattern)
        
        //QLinearGradient linearGradient(0, 0, 100, 100);
        //linearGradient.setColorAt(0.0, Qt::white);
        //linearGradient.setColorAt(0.2, Qt::green);
        //linearGradient.setColorAt(1.0, Qt::black);
        //renderArea->setBrush(linearGradient);

    this->setBrush( QBrush(Qt::green) );
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(500, 400);
}

void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}


void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    static const QPoint points[4] = 
    {
        QPoint(10, 80),
        QPoint(20, 10),
        QPoint(80, 30),
        QPoint(90, 70)
    };

    QRect rect(10, 20, 80, 60);

    QPainterPath path;
    path.moveTo(20, 80);
    path.lineTo(20, 30);
    path.cubicTo(80, 0, 50, 50, 80, 80);

    int startAngle = 20 * 16;
    int arcLength = 120 * 16;

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);

    //if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

////    for (int x = 0; x < width(); x += 100) 
////    {
////        for (int y = 0; y < height(); y += 100) 
////        {
////            painter.save();
////            painter.translate(x, y);
////
////            //if (transformed) 
////            //{
////            //    painter.translate(50, 50);
////            //    painter.rotate(60.0);
////            //    painter.scale(0.6, 0.9);
////            //    painter.translate(-50, -50);
////            //}
//////            painter.drawLine(rect.bottomLeft(), rect.topRight());
////            painter.drawPoints(points, 4);
////            painter.restore();
////        }
////    }
//
//            painter.drawLine(rect.bottomLeft(), rect.topRight());
//            painter.drawLine(0, 0, width() - 1, height() - 1);
//            painter.drawPolyline(points, 4);
//            painter.drawPath( path);
//            //painter.restore();
//
//
//    painter.setRenderHint(QPainter::Antialiasing, false);
//    painter.setPen( palette().dark().color() );
//    painter.setBrush(Qt::NoBrush);
//    painter.drawRect( QRect(0, 0, width() - 1, height() - 1));
//    painter.drawRect( QRect(5, 5, width() - 1-10, height() - 1 -10));
//
//    QString strW = QString::number( this->width() );
//    QString strH = QString::number( this->height() );
//    painter.drawText( QPointF(100,50), strW + " x " + strH );
//
//    //layer.Paint( QRectF(0,0,width(), height()), &painter);

    if ( mpData != nullptr )
    {
        for ( auto& ch : mpData->mtabCh )
            ch.Paint( this);
    }
}

void RenderArea::resizeEvent(QResizeEvent *event)
{
}

void RenderArea::mousePressEvent( QMouseEvent *event)
{
    this->setCursor( Qt::OpenHandCursor);
}

void RenderArea::mouseReleaseEvent( QMouseEvent *event)
{
    this->setCursor( Qt::ArrowCursor);
}

void RenderArea::mouseMoveEvent( QMouseEvent *event)
{
}

void RenderArea::wheelEvent( QWheelEvent *event)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class GraphView
////////////////////////////////////////////////////////////////////////////////////////////////////

GraphView::GraphView( QWidget* parent) 
	: QWidget(parent)
{
	mpToolBar = new QToolBar(this);
	mpToolBar->setFixedHeight(22);

    mptestAct = new QAction(tr("&Test"), this);
    //mptestAct->setShortcuts(QKeySequence::New);
    mptestAct->setStatusTip(tr("Create a new file"));
    //connect(mptestAct, &QAction::triggered, this, &MainWindow::newFile);
    mpToolBar->addAction(mptestAct);

	mpRenderWidget = new RenderArea(&mData,this);


    //mpScene = new PlotScene( this);
    //mpScene->setSceneRect(QRectF(0, 0, 1500, 1000));

    //mpView = new QGraphicsView(mpScene);


	QVBoxLayout* pVBox = new QVBoxLayout(this);
	pVBox->addWidget( mpToolBar);
	pVBox->addWidget( mpRenderWidget);

    //pVBox->addWidget(mpView);
    //mpView->scale( 0.25, 0.25 );

    pVBox->setContentsMargins(1,1,1,1);
	pVBox->setSpacing(1);
	setLayout(pVBox);

    mData.mtabCh.push_back( Channel(0,"test") );
    mData.mtabCh.front().InitDummy();

}


void GraphView::updateData( Document* mpDoc)
{
    if ( mpDoc != nullptr)
    {
        qDebug() << "GraphView::updateData - called";
        const MeasurementData& msData = mpDoc->cData();
    }
}